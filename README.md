# XR Camera Rotation 보정을 위한 Plugin

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

## 개요

확장된 영역을 렌더링하는 컴퓨터의 가상 카메라(Cine Camera Actor)에서 보이는 화면과 LED Wall에 렌더링하는 컴퓨터의 가상 카메라에서 보이는 화면이 맞지 않을때, 확장된 영역을 렌더링하는데 사용되는 가상 카메라 회전값을 보정하여 두 영역이 경계 없이 맞도록 하기 위한 기능을 제공해 주는 Plugin 개발.

## 구성

3차원 공간에서 특정 위치에 보정을 위한 값을 설정 해야 하므로 Octree 공간 분할 알고리즘을 사용하여 효율적인 검색이 가능하도록 하면서 보정값이 존재하는 영역만 분류 가능하여 메모리 사용도 효율적으로 할 수 있게 구성.

![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/Result_ScreenShot.png)

## 사용법

(1) 3인칭 시점을 위한 Player Start와 가상 카메라가 있는 공간을 생성함.

(2) XRCameraCalibration 컨텐츠 폴더에서 BP_VXROctreeController 블루프린터 Actor를 보정을 위한 공간 중심에 배치함.

![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/BP_VXROctreeController.png)

(3) BP_VXROctreeController의 Detail 메뉴에서 보정 공간의 절반 크기를 Extent 항목에 설정 해줌. Octree의 Leaf Node(제일 작은 크기의 사각 영역)의 개수는 8의 n승으로 계산 할 수 있음. 여기서 n은 Octree의 Max Depth값. 그리고 Max Elements 값은 Leaf Node안에 만들 수 있는 보정값을 담는 데이터 생성 갯수를 나타냄. **[(1)번 항목 참고]**

![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/BP_VXROctreeController_Detail.png)

(4) Octree가 Visualized 되도록 Use Debug Draw 항목을 선택 할 수 있음. 또한 Visualized Octree의 색상을 지정 할 수 있음.**[(2)번 항목 참고]**

(5) 설정된 보정값들을 저장하기 위해 저장될 위치 설정 및 파일 이름을 지정 할 수 있음.**[(3)번 항목 참고]**

(6) Editor Play를 실행하면 BP_VXROctreeController가 작동 됨.

## 플러그인 API

* 가상 카메라가 위치한 곳에 보정값을 설정할 데이터 Actor를 Octree에 추가하는 BP 함수.

  ![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/API_InsertToOctree.png)

* 가상 카메라 위치에서 가까운 두 보정값을 설정한 데이터 Actor를 얻어와 가상 카메라가 위치한 곳에서 보정값을 얻어오는  BP 함수.

  ![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/API_GetCollectCameraRotationFromRootOctree.png)

* 보정값을 설정한 데이터 Actor들의 정보를 파일로 저장하는 BP 함수.

  ![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/API_SaveOctreeElementDatas.png)

* 파일로 저장된 보정값 데이터 Actor들을 읽어와 적용하는 BP 함수.

  ![](https://github.com/Devcoder-IndieWorks/XRCameraCalibSample/blob/master/Images/API_LoadOctreeElementDatas.png)