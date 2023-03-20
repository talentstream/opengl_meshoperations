# opengl_meshoperations

### 实现功能

- 使用tinyobjloader导入obj √
- 使用BlinnPhong着色模型进行染色 √
- 对导入的Mesh网格进行MeshOperations 
- Loop Subdivision : 1.先将每个三角形一分为四，取每条边的中点。2.算出每个中点应该在的位置 3.最后将原本存在的点进行位置更新