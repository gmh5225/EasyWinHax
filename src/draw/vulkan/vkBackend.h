#pragma once
#include "vkDefs.h"
#include "..\Vertex.h"
#include "..\IBackend.h"
#include <Windows.h>

namespace hax {

	namespace draw {

		namespace vk {

			typedef struct VulkanInitData {
				PFN_vkQueuePresentKHR pVkQueuePresentKHR;
				VkDevice hDevice;
			}VulkanInitData;

			bool getVulkanInitData(VulkanInitData* initData);

			class Backend : public IBackend {
			private:
				typedef struct BufferData {
					VkBuffer hVertexBuffer;
					VkBuffer hIndexBuffer;
					VkDeviceMemory hVertexMemory;
					VkDeviceMemory hIndexMemory;
					Vertex* pLocalVertexBuffer;
					uint32_t* pLocalIndexBuffer;
					uint32_t vertexBufferSize;
					uint32_t indexBufferSize;
					uint32_t curOffset;
				}BufferData;

				typedef struct ImageData {
					VkCommandBuffer hCommandBuffer;
					VkImageView hImageView;
					VkFramebuffer hFrameBuffer;
					BufferData triangleListBufferData;
					BufferData pointListBufferData;
					VkFence hFence;
				}ImageData;

				VkQueue _hQueue;
				const VkPresentInfoKHR* _phPresentInfo;
				VkDevice _hDevice;

				HMODULE _hVulkan;
				HWND _hMainWindow;
				VkInstance _hInstance;

				union {
					Functions _f;
					void* _fPtrs[sizeof(Functions) / sizeof(void*)];
				};

				VkPhysicalDevice _hPhysicalDevice;
				uint32_t _graphicsQueueFamilyIndex;
				VkRenderPass _hRenderPass;
				VkCommandPool _hCommandPool;
				VkShaderModule _hShaderModuleVert;
				VkShaderModule _hShaderModuleFrag;
				VkDescriptorSetLayout _hDescriptorSetLayout;
				VkPipelineLayout _hPipelineLayout;
				VkPipeline _hTriangleListPipeline;
				VkPipeline _hPointListPipeline;
				VkPhysicalDeviceMemoryProperties _memoryProperties;
				VkQueue _hFirstGraphicsQueue;

				ImageData* _pImageDataArray;
				uint32_t _imageCount;
				VkDeviceSize _bufferAlignment;
				ImageData* _pCurImageData;
				RECT _viewport;

			public:
				Backend();

				~Backend();

				// Sets the arguments of the current call of the hooked function.
				//
				// Parameters:
				// 
				// [in] pArg1:
				// Pass the VkQueue.
				//
				// [in] pArg2:
				// Pass the VkPresentInfoKHR*.
				//
				// [in] pArg3:
				// Pass the device handle that was retrieved by vk::getVulkanInitData().
				virtual void setHookArguments(void* pArg1 = nullptr, const void* pArg2 = nullptr, void* pArg3 = nullptr) override;

				// Initializes the backend. Should be called by an Engine object until success.
				// 
				// Return:
				// True on success, false on failure.
				virtual bool initialize() override;

				// Starts a frame within a hook. Should be called by an Engine object every frame at the begin of the hook.
				// 
				// Return:
				// True on success, false on failure.
				virtual bool beginFrame() override;

				// Ends the current frame within a hook. Should be called by an Engine object every frame at the end of the hook.
				virtual void endFrame() override;

				// Gets the resolution of the current frame. Should be called by an Engine object.
				//
				// Parameters:
				// 
				// [out] frameWidth:
				// Pointer that receives the current frame width in pixel.
				//
				// [out] frameHeight:
				// Pointer that receives the current frame height in pixel.
				virtual void getFrameResolution(float* frameWidth, float* frameHeight) override;

			private:
				bool getProcAddresses();
				bool createRenderPass();
				bool createCommandPool();
				VkPipeline createPipeline(VkPrimitiveTopology topology);
				VkShaderModule createShaderModule(const BYTE shader[], size_t size) const;
				bool createPipelineLayout();
				bool createDescriptorSetLayout();
				bool resizeImageDataArray(VkSwapchainKHR hSwapchain, uint32_t imageCount);
				void destroyImageDataArray();
				void destroyImageData(ImageData* pImageData) const;
				bool createFramebuffers(VkSwapchainKHR hSwapchain);
				void destroyFramebuffers();
				bool createBufferData(BufferData* pBufferData, uint32_t vertexCount);
				void destroyBufferData(BufferData* pBufferData) const;
				bool createBuffer(VkBuffer* phBuffer, VkDeviceMemory* phMemory, uint32_t* pSize, VkBufferUsageFlagBits usage);
				uint32_t getMemoryTypeIndex(uint32_t typeBits) const;
				bool mapBufferData(BufferData* pBufferData) const;
				bool beginCommandBuffer(VkCommandBuffer hCommandBuffer) const;
				void beginRenderPass(VkCommandBuffer hCommandBuffer, VkFramebuffer hFramebuffer) const;
				void copyToBufferData(BufferData* pBufferData, const Vector2 data[], uint32_t count, rgb::Color color, Vector2 offset = { 0.f, 0.f });
				bool resizeBufferData(BufferData* pBufferData, uint32_t newVertexCount);
				void drawBufferData(BufferData* pBufferData, VkCommandBuffer hCommandBuffer) const;
			};

		}

	}

}