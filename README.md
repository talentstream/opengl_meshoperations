# opengl_meshoperations

> 环境基于VS 2022

一个使用tinyobjloader来读取模型，用blinnphong着色模型进行渲染，可以进行三角面的曲面细分

----- 

### 实现功能

- 使用tinyobjloader导入obj √
- 使用BlinnPhong着色模型进行着色 √
- 对导入的Mesh网格进行MeshOperations 
- Loop Subdivision : 1.先将每个三角形一分为四，取每条边的中点。2.算出每个中点应该在的位置 3.最后将原本存在的点进行位置更新 √

-----

### 环境配置

- 添加glfw、glad、glm库
- 添加tinyobjloader.h库

-----

### 结果展示

#### 正方体：

细分0次

![1](https://user-images.githubusercontent.com/72556475/226891649-d2f1fb1a-102f-4ff6-9e28-9e1f888400f8.png)

细分1次

![2](https://user-images.githubusercontent.com/72556475/226891675-a6a77c29-5d9f-4deb-83db-b2b6d6f2a2e4.png)

细分2次

![3](https://user-images.githubusercontent.com/72556475/226891683-972fbc9b-c884-4a01-8662-9da82c460edc.png)

#### 猫：

细分0次

![image](https://user-images.githubusercontent.com/72556475/226891906-fe83d77b-84be-4a72-a948-f1c6f5d82006.png)

细分1次

![image](https://user-images.githubusercontent.com/72556475/226892236-ebbb5cf8-b8e1-42d8-b903-dc2f0bb654fe.png)

细分2次

![image](https://user-images.githubusercontent.com/72556475/226892314-b3c589e9-e2fa-4322-8356-8fe87b476dc0.png)
