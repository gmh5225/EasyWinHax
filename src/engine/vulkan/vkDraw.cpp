#include "vkDraw.h"
#include "..\..\proc.h"

namespace hax {

	namespace vk {

		Draw::Draw() : _hVulkan{}, _pVkDestroyInstance{}, _pVkEnumeratePhysicalDevices{},
			_pVkGetPhysicalDeviceProperties{}, _pVkGetPhysicalDeviceQueueFamilyProperties{}, _pVkCreateDevice{}, _pVkDestroyDevice{},
			_pAllocator{}, _hInstance{}, _hPhysicalDevice{}, _queueFamily{}, _hDevice{},
			_isInit{} {}


		Draw::~Draw() {

			if (this->_pVkDestroyDevice && this->_hDevice) {
				this->_pVkDestroyDevice(this->_hDevice, this->_pAllocator);
			}
			
			if (this->_pVkDestroyInstance && this->_hInstance) {
				this->_pVkDestroyInstance(this->_hInstance, this->_pAllocator);
			}

		}


		void Draw::beginDraw(Engine* pEngine) {

			if (!this->_isInit) {
				this->_hVulkan = proc::in::getModuleHandle("vulkan-1.dll");

				if (!this->_hVulkan) return;

				if (!this->createInstance()) return;

				if (!this->getProcAddresses()) return;

				if (!this->selectGpu()) return;

				if (!this->getQueueFamily()) return;

				if (!this->createDevice()) return;

				this->_isInit = true;
			}

			return;
		}


		void Draw::endDraw(const Engine* pEngine) {
			
			return;
		}


		void Draw::drawString(const void* pFont, const Vector2* pos, const char* text, rgb::Color color) {


			return;
		}


		void Draw::drawTriangleList(const Vector2 corners[], UINT count, rgb::Color color) {

			return;
		}


		bool Draw::createInstance() {
			PFN_vkCreateInstance pVkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(proc::in::getProcAddress(this->_hVulkan, "vkCreateInstance"));

			if (!pVkCreateInstance) return false;

			constexpr const char* EXTENSION = "VK_KHR_surface";

			VkInstanceCreateInfo createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.enabledExtensionCount = 1u;
			createInfo.ppEnabledExtensionNames = &EXTENSION;

			return pVkCreateInstance(&createInfo, this->_pAllocator, &this->_hInstance) == VkResult::VK_SUCCESS;
		}


		bool Draw::getProcAddresses() {
			PFN_vkGetInstanceProcAddr pVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(proc::in::getProcAddress(this->_hVulkan, "vkGetInstanceProcAddr"));

			if (!pVkGetInstanceProcAddr) return false;

			this->_pVkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(pVkGetInstanceProcAddr(this->_hInstance, "vkDestroyInstance"));
			this->_pVkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(pVkGetInstanceProcAddr(this->_hInstance, "vkEnumeratePhysicalDevices"));
			this->_pVkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(pVkGetInstanceProcAddr(this->_hInstance, "vkGetPhysicalDeviceProperties"));
			this->_pVkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(pVkGetInstanceProcAddr(this->_hInstance, "vkGetPhysicalDeviceQueueFamilyProperties"));
			this->_pVkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(pVkGetInstanceProcAddr(this->_hInstance, "vkCreateDevice"));
			this->_pVkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(pVkGetInstanceProcAddr(this->_hInstance, "vkDestroyDevice"));

			if (
				!this->_pVkDestroyInstance || !this->_pVkEnumeratePhysicalDevices || !this->_pVkGetPhysicalDeviceProperties ||
				!this->_pVkGetPhysicalDeviceQueueFamilyProperties || !this->_pVkCreateDevice || !this->_pVkDestroyDevice
			) return false;

			return true;
		}


		bool Draw::selectGpu() {
			uint32_t gpuCount = 0u;

			if (this->_pVkEnumeratePhysicalDevices(this->_hInstance, &gpuCount, nullptr) != VkResult::VK_SUCCESS) return false;

			if (!gpuCount) return false;

			const uint32_t bufferSize = gpuCount;
			VkPhysicalDevice* const pPhysicalDevices = new VkPhysicalDevice[bufferSize]{};

			if (this->_pVkEnumeratePhysicalDevices(this->_hInstance, &gpuCount, pPhysicalDevices) != VkResult::VK_SUCCESS) return false;

			for (uint32_t i = 0u; i < bufferSize; i++) {
				VkPhysicalDeviceProperties properties{};
				this->_pVkGetPhysicalDeviceProperties(pPhysicalDevices[i], &properties);

				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
					this->_hPhysicalDevice = pPhysicalDevices[i];
					break;
				}
			}

			delete[] pPhysicalDevices;

			return true;
		}


		bool Draw::getQueueFamily() {
			uint32_t propertiesCount = 0u;
			this->_pVkGetPhysicalDeviceQueueFamilyProperties(this->_hPhysicalDevice, &propertiesCount, nullptr);

			if (!propertiesCount) return false;

			const uint32_t bufferSize = propertiesCount;
			VkQueueFamilyProperties* const pProperties = new VkQueueFamilyProperties[bufferSize]{};

			this->_pVkGetPhysicalDeviceQueueFamilyProperties(this->_hPhysicalDevice, &propertiesCount, pProperties);

			this->_queueFamily = 0xFFFFFFFF;

			for (uint32_t i = 0u; i < bufferSize; i++) {

				if (pProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					this->_queueFamily = i;
					break;
				}

			}

			if (this->_queueFamily == 0xFFFFFFFF) return false;

			return true;
		}


		bool Draw::createDevice() {
			constexpr const char* EXTENSION = "VK_KHR_swapchain";
			constexpr float QUEUE_PRIORITY = 1.f;

			VkDeviceQueueCreateInfo queueInfo = {};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = this->_queueFamily;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &QUEUE_PRIORITY;

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = 1u;
			createInfo.pQueueCreateInfos = &queueInfo;
			createInfo.enabledExtensionCount = 1u;
			createInfo.ppEnabledExtensionNames = &EXTENSION;

			return this->_pVkCreateDevice(this->_hPhysicalDevice, &createInfo, this->_pAllocator, &this->_hDevice) == VkResult::VK_SUCCESS;
		}

	}

}