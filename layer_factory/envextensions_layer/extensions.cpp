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

#include "extensions.h"

#include <sstream>

VkResult EnvExtensions::PreCallCreateInstance( const VkInstanceCreateInfo *pCreateInfo,
                                               const VkAllocationCallbacks *pAllocator,
                                               VkInstance *pInstance )
{
    // Save caller's parameters
    mInstanceLock.lock();
    mSavedInstanceExtensionCount = pCreateInfo->enabledExtensionCount;
    mSavedInstanceExtensionNames = pCreateInfo->ppEnabledExtensionNames;

    // Prepare new extension list - start with the original request
    mPatchedInstanceExtensions.clear();
    for ( uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i ) {
        mPatchedInstanceExtensions.push_back( pCreateInfo->ppEnabledExtensionNames[i] );
    }

    // Append the extra extensions
    char *pEnvExtensions = std::getenv("VK_ENVEXTENSIONS_INSTANCE_APPEND");
    if ( pEnvExtensions) {
        std::string token;
        std::stringstream splitStream( pEnvExtensions );

        mExtraInstanceExtensions.clear();
        while ( std::getline( splitStream, token, ':' ) ) {
            // Stored as a so we maintain the lifetime until the call is complete
            mExtraInstanceExtensions.push_back( token );
        }
    }

    for ( uint32_t i = 0; i < mExtraInstanceExtensions.size(); ++i ) {
        mPatchedInstanceExtensions.push_back( mExtraInstanceExtensions[i].c_str() );
    }

    for ( uint32_t i = 0; i < mPatchedInstanceExtensions.size(); ++i ) {
        printf("instance[%d] = %s\n", i, mPatchedInstanceExtensions[i] );
    }

    // Patch with our extra extensions
    VkInstanceCreateInfo *pCreateInfoPatched = const_cast<VkInstanceCreateInfo*>( pCreateInfo );
    pCreateInfoPatched->enabledExtensionCount = mPatchedInstanceExtensions.size();
    pCreateInfoPatched->ppEnabledExtensionNames = mPatchedInstanceExtensions.data();

    return VK_SUCCESS;
}

VkResult EnvExtensions::PostCallCreateInstance( const VkInstanceCreateInfo *pCreateInfo,
                                                const VkAllocationCallbacks *pAllocator,
                                                VkInstance *pInstance )
{
    // Restore caller's parameters
    VkInstanceCreateInfo *pCreateInfoPatched = const_cast<VkInstanceCreateInfo*>( pCreateInfo );
    pCreateInfoPatched->enabledExtensionCount = mSavedInstanceExtensionCount;
    pCreateInfoPatched->ppEnabledExtensionNames = mSavedInstanceExtensionNames;

    mExtraInstanceExtensions.clear();
    mPatchedInstanceExtensions.clear();

    mInstanceLock.unlock();

    return VK_SUCCESS;
}

VkResult EnvExtensions::PreCallCreateDevice( VkPhysicalDevice physicalDevice,
                                             const VkDeviceCreateInfo *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDevice *pDevice )
{
    // Save caller's parameters
    mDeviceLock.lock();
    mSavedDeviceExtensionCount = pCreateInfo->enabledExtensionCount;
    mSavedDeviceExtensionNames = pCreateInfo->ppEnabledExtensionNames;

    // Prepare new extension list - start with the original request
    mPatchedDeviceExtensions.clear();
    for ( uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i ) {
        mPatchedDeviceExtensions.push_back( pCreateInfo->ppEnabledExtensionNames[i] );
    }

    // Append the extra extensions
    char *pEnvExtensions = std::getenv("VK_ENVEXTENSIONS_DEVICE_APPEND");
    if ( pEnvExtensions) {
        std::string token;
        std::stringstream splitStream( pEnvExtensions );

        mExtraDeviceExtensions.clear();
        while ( std::getline( splitStream, token, ':' ) ) {
            // Stored as a so we maintain the lifetime until the call is complete
            mExtraDeviceExtensions.push_back( token );
        }
    }

    for ( uint32_t i = 0; i < mExtraDeviceExtensions.size(); ++i ) {
        mPatchedDeviceExtensions.push_back( mExtraDeviceExtensions[i].c_str() );
    }

    for ( uint32_t i = 0; i < mPatchedDeviceExtensions.size(); ++i ) {
        printf("device[%d] = %s\n", i, mPatchedDeviceExtensions[i] );
    }

    // Patch with our extra extensions
    VkDeviceCreateInfo *pCreateInfoPatched = const_cast<VkDeviceCreateInfo*>( pCreateInfo );
    pCreateInfoPatched->enabledExtensionCount = mPatchedDeviceExtensions.size();
    pCreateInfoPatched->ppEnabledExtensionNames = mPatchedDeviceExtensions.data();

    return VK_SUCCESS;
}

VkResult EnvExtensions::PostCallCreateDevice( VkPhysicalDevice physicalDevice,
                                              const VkDeviceCreateInfo *pCreateInfo,
                                              const VkAllocationCallbacks *pAllocator,
                                              VkDevice *pDevice )
{
    // Restore caller's parameters
    VkDeviceCreateInfo *pCreateInfoPatched = const_cast<VkDeviceCreateInfo*>( pCreateInfo );
    pCreateInfoPatched->enabledExtensionCount = mSavedDeviceExtensionCount;
    pCreateInfoPatched->ppEnabledExtensionNames = mSavedDeviceExtensionNames;

    mExtraDeviceExtensions.clear();
    mPatchedDeviceExtensions.clear();

    mDeviceLock.unlock();

    return VK_SUCCESS;
}

EnvExtensions env_extensions_layer;
