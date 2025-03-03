#include "PointList.h"
#include <random>


// Initialise buffers
PointList::PointList(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}

// Release resources.
PointList::~PointList()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build shape and fill buffers.
void PointList::initBuffers(ID3D11Device* device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	
	// Number of particles 

	float numofparticles = 99999.0f;

	//Initialise vertex and index counts
	vertexCount = numofparticles;
	indexCount = numofparticles;

	//Store vertices and indices
	VertexType_Colour* vertices = new VertexType_Colour[numofparticles];
	unsigned long* indices = new unsigned long[numofparticles];

	//Scatter points around terrain with random positions based on deviations
	for (int i = 0; i < numofparticles; i++)
	{
		std::random_device rd; 
		std::mt19937 gen(rd()); 
		std::uniform_real_distribution<float> dist(-20.0f, 2000.0f);


		std::random_device rd1;
		std::mt19937 gen1(rd1());
		std::uniform_real_distribution<float> dist1(0.0f, 2000.0f);


		std::random_device rd2;
		std::mt19937 gen2(rd2());
		std::uniform_real_distribution<float> dist2(0, 500.0f);

		vertices[i].position.x = dist(gen);     // Random X position
		vertices[i].position.y = dist1(gen1);   // Random Y position
		vertices[i].position.z = dist2(gen2);   // Random Z position
		vertices[i].colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);// Set default color to white
	}

	// Set indices

	for (int i = 0; i < numofparticles; i++)
	{
		indices[i] = i;
	}


	// Description for the vertex buffer

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType_Colour) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;


	// Create the vertex buffer
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	// Description for the index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.

		// Create the index buffer

	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

// Send Geometry data to the GPU
void PointList::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType_Colour);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
