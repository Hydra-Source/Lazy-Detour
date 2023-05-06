#include <stdexcept>
#include <Windows.h>
#include "minhook/MinHook.h"

class detour {
    public:
        detour(const char* name, void* ptr, void* detour) : m_name(name), m_ptr(ptr), m_detour(detour), m_status(true)
        {
            if (MH_CreateHook(m_ptr, m_detour, &m_orig) != MH_OK) {
                   m_status = false;
                  throw std::runtime_error("Failed to create hook: " + std::to_string(m_name));
            }  
        }

        void enable()
        {
            if (m_status && MH_EnableHook(m_ptr) != MH_OK) {
                 m_status = false;
                 throw std::runtime_error("Failed to enable hook: " + std::to_string(m_name));
            }
        }

        void disable()
        {
            if (m_status) {
                if (MH_DisableHook(m_ptr) == MH_OK) {
                    if (MH_RemoveHook(m_ptr) != MH_OK) {
                        m_status = false;
                        throw std::runtime_error("Failed to remove hook: " + std::to_string(m_name));
                    }
                }
                else {
                        m_status = false;
                        throw std::runtime_error("Failed to disable hook: " + std::to_string(m_name));
                }              
            }
        }

       
        void* get_original() const {
            return m_orig;
        }

    private:
        const char* m_name;

        void* m_ptr;
        void* m_detour;
        void* m_orig;

        bool m_status;
    };
    
    inline std::map<std::string, std::unique_ptr<detour>> g_hook_map;

    template <typename func>
    static void create_and_enable_hook(const std::string& name, func original_func, func detour_func) {
        detour* new_hook = new detour(name.c_str(), reinterpret_cast<void*>(original_func), reinterpret_cast<void*>(detour_func));
        g_hook_map.insert(std::make_pair(name, std::unique_ptr<detour>(new_hook)));
        g_hook_map[name]->enable();
    }

    static void remove_all_hooks()
    {
        for (const auto& [name, detour] : g_hook_map) {
            g_hook_map[name]->disable();
        }
    }

    static void remove_hook(std::string& name)
    {
        auto it = g_hook_map.find(name);
        if (it != g_hook_map.end()) {
            it->second->disable();
            g_hook_map.erase(it);
        }
    }

    template<typename function>
    static function get_original(const std::string& name)
    {
        auto it = g_hook_map.find(name);

        if (it != g_hook_map.end())
        {
            const auto original = it->second->get_original();
            
            return reinterpret_cast<function>(original);
        }
    }