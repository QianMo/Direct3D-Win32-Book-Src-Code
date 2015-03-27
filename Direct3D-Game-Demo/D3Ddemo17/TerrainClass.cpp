//=============================================================================
// Name: TerrainClass.cpp
//	Des: 一个封装了三维地形系统的类的源文件
// 2013年 3月17日  Create by 浅墨 
//=============================================================================

#include "TerrainClass.h"

//-----------------------------------------------------------------------------
// Desc: 构造函数
//-----------------------------------------------------------------------------
TerrainClass::TerrainClass(IDirect3DDevice9* pd3dDevice)
{
	//给各个成员变量赋初值
    m_pd3dDevice = pd3dDevice;
    m_pTexture = NULL;
    m_pIndexBuffer = NULL;
    m_pVertexBuffer = NULL;
    m_nCellsPerRow = 0;
    m_nCellsPerCol = 0;
    m_nVertsPerRow = 0;
    m_nVertsPerCol = 0;
    m_nNumVertices = 0;
    m_fTerrainWidth = 0.0f;
    m_fTerrainDepth = 0.0f;
    m_fCellSpacing = 0.0f;
    m_fHeightScale = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: TerrainClass::LoadTerrainFromFile()
// Desc: 加载地形高度信息以及纹理
//--------------------------------------------------------------------------------------
BOOL TerrainClass::LoadTerrainFromFile(wchar_t *pRawFileName, wchar_t *pTextureFile) 
{
    // 从文件中读取高度信息
    std::ifstream inFile;     
    inFile.open(pRawFileName, std::ios::binary);   //用二进制的方式打开文件

    inFile.seekg(0,std::ios::end);							//把文件指针移动到文件末尾
    std::vector<BYTE> inData(inFile.tellg());			//用模板定义一个vector<BYTE>类型的变量inData并初始化，其值为缓冲区当前位置，即缓冲区大小

    inFile.seekg(std::ios::beg);								//将文件指针移动到文件的开头，准备读取高度信息
    inFile.read((char*)&inData[0], inData.size());	//关键的一步，读取整个高度信息
    inFile.close();													//操作结束，可以关闭文件了

    m_vHeightInfo.resize(inData.size());					//将m_vHeightInfo尺寸取为缓冲区的尺寸
	//遍历整个缓冲区，将inData中的值赋给m_vHeightInfo
    for (unsigned int i=0; i<inData.size(); i++)		
        m_vHeightInfo[i] = inData[i];

    // 加载地形纹理
    if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile, &m_pTexture)))
        return FALSE;

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: TerrainClass::InitTerrain()
// Desc: 初始化地形的高度, 填充顶点和索引缓存
//--------------------------------------------------------------------------------------
BOOL TerrainClass::InitTerrain(INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale) 
{
    m_nCellsPerRow  = nRows;  //每行的单元格数目
    m_nCellsPerCol  = nCols;  //每列的单元格数目
    m_fCellSpacing  = fSpace;	//单元格间的间距
    m_fHeightScale  = fScale; //高度缩放系数
    m_fTerrainWidth = nRows * fSpace;  //地形的宽度
    m_fTerrainDepth = nCols * fSpace;  //地形的深度
    m_nVertsPerRow  = m_nCellsPerCol + 1;  //每行的顶点数
    m_nVertsPerCol  = m_nCellsPerRow + 1; //每列的顶点数
    m_nNumVertices  = m_nVertsPerRow * m_nVertsPerCol;  //顶点总数

    // 通过一个for循环，逐个把地形原始高度乘以缩放系数，得到缩放后的高度
    for(unsigned int i=0; i<m_vHeightInfo.size(); i++)
        m_vHeightInfo[i] *= m_fHeightScale;
	//---------------------------------------------------------------
    // 处理地形的顶点
	//---------------------------------------------------------------
	//1，创建顶点缓存
    if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_nNumVertices * sizeof(TERRAINVERTEX), 
        D3DUSAGE_WRITEONLY, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, 0)))
        return FALSE;
	//2，加锁
    TERRAINVERTEX *pVertices = NULL;
    m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	//3，访问，赋值
    FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX =  m_fTerrainWidth / 2.0f;		//指定起始点和结束点的X坐标值
    FLOAT fStartZ =  m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;	//指定起始点和结束点的Z坐标值
    FLOAT fCoordU = 3.0f / (FLOAT)m_nCellsPerRow;     //指定纹理的横坐标值
    FLOAT fCoordV = 3.0f / (FLOAT)m_nCellsPerCol;		//指定纹理的纵坐标值

    int nIndex = 0, i = 0, j = 0;
    for (float z = fStartZ; z > fEndZ; z -= m_fCellSpacing, i++)		//Z坐标方向上起始顶点到结束顶点行间的遍历
    {
        j = 0;
        for (float x = fStartX; x < fEndX; x += m_fCellSpacing, j++)	//X坐标方向上起始顶点到结束顶点行间的遍历
        {
            nIndex = i * m_nCellsPerRow + j;		//指定当前顶点在顶点缓存中的位置
            pVertices[nIndex] = TERRAINVERTEX(x, m_vHeightInfo[nIndex], z, j*fCoordU, i*fCoordV); //把顶点位置索引在高度图中对应的各个顶点参数以及纹理坐标赋值给赋给当前的顶点
            nIndex++;											//索引数自加1
        }
    }
	//4，解锁
    m_pVertexBuffer->Unlock();

	//---------------------------------------------------------------
    // 处理地形的索引
	//---------------------------------------------------------------
	//1.创建索引缓存
    if (FAILED(m_pd3dDevice->CreateIndexBuffer(m_nNumVertices * 6 *sizeof(WORD), 
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, 0)))
        return FALSE;
	//2.加锁
    WORD* pIndices = NULL;
    m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	//3.访问，赋值
    nIndex = 0;
    for(int row = 0; row < m_nCellsPerRow-1; row++)   //遍历每行
    {
        for(int col = 0; col < m_nCellsPerCol-1; col++)  //遍历每列
        {
			//三角形ABC的三个顶点
            pIndices[nIndex]   =  row * m_nCellsPerRow + col;			//顶点A
            pIndices[nIndex+1] =  row * m_nCellsPerRow + col + 1;  //顶点B
            pIndices[nIndex+2] = (row+1) * m_nCellsPerRow + col;	//顶点C
			//三角形CBD的三个顶点
            pIndices[nIndex+3] = (row+1) * m_nCellsPerRow + col;		//顶点C
            pIndices[nIndex+4] =  row * m_nCellsPerRow + col + 1;		//顶点B
            pIndices[nIndex+5] = (row+1) * m_nCellsPerRow + col + 1;//顶点D
			//处理完一个单元格，索引加上6
            nIndex += 6;  //索引自加6
        }
    }
	//4、解锁
    m_pIndexBuffer->Unlock();

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: TerrainClass::RenderTerrain()
// Desc: 绘制出地形，可以通过第二个参数选择是否绘制出线框
//--------------------------------------------------------------------------------------
BOOL TerrainClass::RenderTerrain(D3DXMATRIX *pMatWorld, BOOL bRenderFrame) 
{
    m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(TERRAINVERTEX));   ///把包含的几何体信息的顶点缓存和渲染流水线相关联  
    m_pd3dDevice->SetFVF(TERRAINVERTEX::FVF);//指定我们使用的灵活顶点格式的宏名称
    m_pd3dDevice->SetIndices(m_pIndexBuffer);//设置索引缓存  
    m_pd3dDevice->SetTexture(0, m_pTexture);//设置纹理

    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);	//关闭光照
    m_pd3dDevice->SetTransform(D3DTS_WORLD, pMatWorld); //设置世界矩阵
    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
        m_nNumVertices, 0, m_nNumVertices * 2);		//绘制顶点

    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);  //打开光照
    m_pd3dDevice->SetTexture(0, 0);	//纹理置空

    if (bRenderFrame)  //如果要渲染出线框的话
    {
        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); //把填充模式设为线框填充
        m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
            m_nNumVertices, 0, m_nNumVertices * 2);	//绘制顶点  
        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	//把填充模式调回实体填充
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// Desc: 析构函数
//-----------------------------------------------------------------------------
TerrainClass::~TerrainClass(void)
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
}