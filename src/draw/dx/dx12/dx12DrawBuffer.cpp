#include "dx12DrawBuffer.h"

namespace hax {

	namespace draw {

		namespace dx12 {

			DrawBuffer::DrawBuffer() : _pDevice{}, _pVertexBufferResource {}, _pIndexBufferResource{} {}


			DrawBuffer::~DrawBuffer() {
				this->destroy();

				return;
			}

			void DrawBuffer::initialize(ID3D12Device* pDevice) {
				this->_pDevice = pDevice;

				return;
			}


			bool DrawBuffer::create(uint32_t vertexCount) {
				this->reset();

				this->_pVertexBufferResource = nullptr;
				this->_pIndexBufferResource = nullptr;

				const uint32_t vertexBufferSize = vertexCount * sizeof(Vertex);

				if (!this->createBuffer(&this->_pVertexBufferResource, vertexBufferSize)) return false;

				this->_vertexBufferSize = vertexBufferSize;

				const uint32_t indexBufferSize = vertexCount * sizeof(uint32_t);

				if (!this->createBuffer(&this->_pIndexBufferResource, indexBufferSize)) return false;

				this->_indexBufferSize = indexBufferSize;

				return true;
			}


			void DrawBuffer::destroy() {

				if (this->_pVertexBufferResource) {
					this->_pVertexBufferResource->Release();
					this->_pVertexBufferResource = nullptr;
				}

				if (this->_pIndexBufferResource) {
					this->_pIndexBufferResource->Release();
					this->_pIndexBufferResource = nullptr;
				}

				this->reset();

				return;
			}


			bool DrawBuffer::map() {

				if (!this->_pLocalVertexBuffer) {
					
					if (FAILED(this->_pVertexBufferResource->Map(0u, nullptr, reinterpret_cast<void**>(&this->_pLocalVertexBuffer)))) return false;

				}
				
				if (!this->_pLocalIndexBuffer) {

					if (FAILED(this->_pVertexBufferResource->Map(0u, nullptr, reinterpret_cast<void**>(&this->_pLocalIndexBuffer)))) return false;

				}

				return true;
			}


			void DrawBuffer::draw() {

				return;
			}


			bool DrawBuffer::createBuffer(ID3D12Resource** ppBufferResource, uint32_t size) const {
				D3D12_HEAP_PROPERTIES heapProperties{};
				heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
				heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

				D3D12_RESOURCE_DESC resourceDesc{};
				resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				resourceDesc.Width = size;
				resourceDesc.Height = 1u;
				resourceDesc.DepthOrArraySize = 1u;
	
				return SUCCEEDED(this->_pDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(ppBufferResource)));
			}

		}

	}

}