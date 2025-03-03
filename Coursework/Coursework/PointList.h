#include "BaseMesh.h"

using namespace DirectX;

class PointList : public BaseMesh
{

public:
	PointList(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~PointList();

	void sendData(ID3D11DeviceContext*, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

protected:
	void initBuffers(ID3D11Device* device);

};