/*
 * Copyright (c) 2018 Valve Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Andres Rodriguez <andresr@valvesoftware.com>
 */

#pragma once

#include <vector>
#include <mutex>

#include "vulkan/vulkan.h"
#include "vk_layer_logging.h"
#include "layer_factory.h"


class EnvExtensions : public layer_factory {
    public:
        // Constructor for state_tracker
        EnvExtensions() {};

        VkResult PreCallCreateInstance( const VkInstanceCreateInfo *pCreateInfo,
                                        const VkAllocationCallbacks *pAllocator,
                                        VkInstance *pInstance ) override;

        VkResult PostCallCreateInstance( const VkInstanceCreateInfo *pCreateInfo,
                                         const VkAllocationCallbacks *pAllocator,
                                         VkInstance *pInstance ) override;

        VkResult PreCallCreateDevice( VkPhysicalDevice physicalDevice,
                                      const VkDeviceCreateInfo *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDevice *pDevice ) override;

        VkResult PostCallCreateDevice( VkPhysicalDevice physicalDevice,
                                       const VkDeviceCreateInfo *pCreateInfo,
                                       const VkAllocationCallbacks *pAllocator,
                                       VkDevice *pDevice ) override;
    private:
        std::mutex mInstanceLock;
        std::vector<const char*> mPatchedInstanceExtensions;
        std::vector<std::string> mExtraInstanceExtensions;
        uint32_t mSavedInstanceExtensionCount;
        const char * const* mSavedInstanceExtensionNames;

        std::mutex mDeviceLock;
        std::vector<const char*> mPatchedDeviceExtensions;
        std::vector<std::string> mExtraDeviceExtensions;
        uint32_t mSavedDeviceExtensionCount;
        const char * const* mSavedDeviceExtensionNames;
};
