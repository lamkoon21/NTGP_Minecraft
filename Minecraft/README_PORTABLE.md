# MineCraft: 프로젝트 내부 의존성 구성

## 빌드 및 실행

1. Visual Studio에서 `MineCraft.sln`을 연다.
2. `Release | x64`를 선택하고 빌드한다.
3. Visual Studio에서 실행하거나 `bin/x64/Release/MineCraft.exe`를 실행한다.

Visual Studio의 **Desktop development with C++** 및 Windows SDK가 빌드에 필요하다.
프로젝트는 해당 Visual Studio에 기본으로 설정된 MSVC 도구 집합을 사용한다.
명령행에서 `/p:PlatformToolset=v143`처럼 명시적으로 지정할 수도 있다.
외부 패키지 다운로드나 CMake 설치는 필요하지 않다.
Win32(솔루션에서는 x86)와 x64, Debug와 Release에 동일한 구성을 적용했다.

## 다른 컴퓨터로 옮기기

- 개발/빌드: 이 `MineCraft` 폴더 전체를 복사한다. `.vs`, `obj`, 기존 빌드 캐시는 필요 없다.
- 실행만 하기: `bin/x64/Release` 폴더 전체를 복사한다. EXE, `resource`, `shader`, `licenses`를 함께 유지한다.
- freeglut/GLEW/GLM을 Windows 또는 Program Files에 복사할 필요가 없다.
- freeglut와 GLEW 및 MSVC 런타임은 정적으로 포함된다. 별도의 freeglut.dll/glew32.dll이 필요 없다.
- Windows와 게임의 GLSL/OpenGL 기능을 지원하는 그래픽 드라이버는 필요하다.

## 변경 내용

- `third_party/freeglut`: 다운로드 폴더의 freeglut 3.8.0 소스 및 라이선스.
- `third_party/glew`: 다운로드 폴더의 GLEW 2.3.1 소스 및 라이선스.
- `third_party/glm`: 다운로드 폴더의 GLM 1.0.3 헤더 및 라이선스.
- freeglut의 Windows C 소스와 `glew.c`를 프로젝트에서 직접 컴파일한다.
  아키텍처와 Debug/Release가 항상 게임의 설정과 일치하며 미리 빌드된 .lib에 의존하지 않는다.
- freeglut의 `config.h`, `src/fg_version.h`는 이 Windows/MSVC 빌드용으로 포함했다.
- `GLEW_STATIC`, `FREEGLUT_STATIC`으로 정적 연결하며 필요한 Windows 시스템 라이브러리는 프로젝트에 명시한다.
- GLM include를 `glm/...`로 정리하고 새 GLM의 확장 헤더를 위해 `GLM_ENABLE_EXPERIMENTAL`을 정의했다.
- 모든 include 경로는 `$(ProjectDir)` 기준이다. C++20을 네 빌드 구성에 통일했다.
- 빌드 후 `resource`와 `shader`를 출력 폴더로 자동 복사한다.
- `main.cpp`의 `SetResourceDirectory()`가 실행 파일 위치를 작업 디렉터리로 지정한다.
- 기존 게임 로직과 리소스는 유지했다. 기존 숫자 형 변환 경고는 남아 있을 수 있다.

새 리소스를 추가/변경하면 다시 빌드하면 된다. 삭제한 리소스의 이전 출력 복사본은 자동 삭제하지 않는다.

## 검증 결과

- Visual Studio 18 / MSVC v145에서 x64 및 Win32의 Debug/Release 빌드 성공.
- 64비트 호스트 빌드 도구를 사용하도록 설정했다.
- 컴파일 의존성 기록에서 GLEW/freeglut/GLM 헤더를 모두 third_party에서 읽는 것을 확인했다.
- x64 Release 실행 파일은 Windows 기본 DLL만 참조한다.
- 한글과 공백이 있는 다른 폴더로 실행 결과물을 복사하여 타이틀과 싱글플레이 지형/텍스처 표시를 확인했다.
- 다른 물리 PC에서의 실행은 검증하지 않았다. 기존 숫자 변환 경고는 유지했다.
