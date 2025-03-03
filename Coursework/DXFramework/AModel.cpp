#include "AModel.h"
#include <iostream>

AModel::AModel(ID3D11Device* ldevice, const std::string& file)
{
	device = ldevice;
	importModel(file);
}

AModel::~AModel()
{

}

void AModel::initBuffers(ID3D11Device* device)
{
	
}

void AModel::importModel(const std::string& pFile)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_MakeLeftHanded |
		aiProcess_FlipUVs |
		aiProcess_ImproveCacheLocality |
		aiProcess_GenSmoothNormals |
		aiProcess_PreTransformVertices |
		aiProcess_FixInfacingNormals);



	if (scene)
	{
		processNode(scene->mRootNode, scene);
	}

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
		return;
	}


	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* (int)vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* (int)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete vertices;
	//vertices = 0;

	//delete indices;
	//indices = 0;
	vertexCount = (int)vertices.size();
	indexCount = (int)indices.size();

	//vertices.clear();
	//indices.clear();
}

void AModel::processScene(const aiScene* scene)
{

}
void AModel::processNode(const aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		//meshes.push_back(this->processMesh(mesh, scene));
		processMesh(mesh, scene);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}
void AModel::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	//---------------------------------

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		XMFLOAT3 vert;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 bitangent;

		vert.x = mesh->mVertices[i].x;
		vert.y = mesh->mVertices[i].y;
		vert.z = mesh->mVertices[i].z;

		if (mesh->HasTextureCoords(0))
		{
			texture.x = (float)mesh->mTextureCoords[0][i].x;
			texture.y = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;


			bitangent.x = mesh->mBitangents[i].x;
			bitangent.y = mesh->mBitangents[i].y;
			bitangent.z = mesh->mBitangents[i].z;

		}

		VertexType ModelData;
		ModelData.position = vert;
		ModelData.texture = texture;
		ModelData.normal = normal;
	    ModelData.tangent = tangent;
		ModelData.bitangent = bitangent;

		vertices.push_back(ModelData);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
}

void AModel::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	
		unsigned int stride;
		unsigned int offset;

		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType);
		offset = 0;

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(top);
}
