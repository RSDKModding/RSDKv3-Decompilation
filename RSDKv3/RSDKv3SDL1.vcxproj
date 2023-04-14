<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>16.0</VCProjectVersion>
    <ProjectGuid>{BD4A4266-8ED9-491E-B4CC-3F63F0D39C1B}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <RootNamespace>RSDKv3</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v142</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v142</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v142</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v142</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LinkIncremental>true</LinkIncremental>
    <OutDir>$(SolutionDir)build\$(Platform)\$(Configuration)\</OutDir>
    <TargetName>Sonic CD</TargetName>
    <IntDir>$(Platform)\$(Configuration)\</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <LinkIncremental>true</LinkIncremental>
    <OutDir>$(SolutionDir)build\$(Platform)\$(Configuration)\</OutDir>
    <TargetName>Sonic CD_64</TargetName>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <LinkIncremental>false</LinkIncremental>
    <OutDir>$(SolutionDir)build\$(Platform)\$(Configuration)\</OutDir>
    <TargetName>Sonic CD</TargetName>
    <IntDir>$(Platform)\$(Configuration)\</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <LinkIncremental>false</LinkIncremental>
    <OutDir>$(SolutionDir)build\$(Platform)\$(Configuration)\</OutDir>
    <TargetName>Sonic CD_64</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <PrecompiledHeader>NotUsing</PrecompiledHeader>
      <WarningLevel>Level1</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <AdditionalIncludeDirectories>$(SolutionDir)dependencies/windows/SDL1/include/;$(SolutionDir)dependencies/windows/libogg/include/;$(SolutionDir)dependencies/windows/libvorbis/include/;$(SolutionDir)dependencies/windows/libtheora/include/;$(SolutionDir)dependencies/all/theoraplay/;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalLibraryDirectories>$(SolutionDir)dependencies/windows/SDL1/lib/$(PlatformTargetAsMSBuildArchitecture)/;$(SolutionDir)dependencies/windows/libvorbis/win32/VS2010/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libtheora/win32/VS2008/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libogg/win32/VS2015/$(Platform)/$(Configuration)/</AdditionalLibraryDirectories>
      <AdditionalDependencies>SDL.lib;SDLmain.lib;libogg.lib;libvorbis_static.lib;libvorbisfile_static.lib;libtheora_static.lib;Shell32.lib;Advapi32.lib</AdditionalDependencies>
    </Link>
    <PostBuildEvent>
      <Command>copy /Y "$(SolutionDir)dependencies\windows\SDL1\lib\$(PlatformTargetAsMSBuildArchitecture)\SDL.dll" "$(OutDir)" </Command>
    </PostBuildEvent>
    <PostBuildEvent>
      <Message>copying SDL1 library DLL to the build directory. build should fail if it's not found</Message>
    </PostBuildEvent>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <PrecompiledHeader>NotUsing</PrecompiledHeader>
      <WarningLevel>Level1</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <AdditionalIncludeDirectories>$(SolutionDir)dependencies/windows/SDL1/include/;$(SolutionDir)dependencies/windows/libogg/include/;$(SolutionDir)dependencies/windows/libvorbis/include/;$(SolutionDir)dependencies/windows/libtheora/include/;$(SolutionDir)dependencies/all/theoraplay/;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalLibraryDirectories>$(SolutionDir)dependencies/windows/SDL1/lib/$(PlatformTargetAsMSBuildArchitecture)/;$(SolutionDir)dependencies/windows/libvorbis/win32/VS2010/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libtheora/win32/VS2008/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libogg/win32/VS2015/$(Platform)/$(Configuration)/</AdditionalLibraryDirectories>
      <AdditionalDependencies>SDL.lib;SDLmain.lib;libogg.lib;libvorbis_static.lib;libvorbisfile_static.lib;libtheora_static.lib;Shell32.lib;Advapi32.lib</AdditionalDependencies>
    </Link>
    <PostBuildEvent>
      <Command>copy /Y "$(SolutionDir)dependencies\windows\SDL1\lib\$(PlatformTargetAsMSBuildArchitecture)\SDL.dll" "$(OutDir)" </Command>
    </PostBuildEvent>
    <PostBuildEvent>
      <Message>copying SDL1 library DLL to the build directory. build should fail if it's not found</Message>
    </PostBuildEvent>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <PrecompiledHeader>NotUsing</PrecompiledHeader>
      <WarningLevel>Level1</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <AdditionalIncludeDirectories>$(SolutionDir)dependencies/windows/SDL1/include/;$(SolutionDir)dependencies/windows/libogg/include/;$(SolutionDir)dependencies/windows/libvorbis/include/;$(SolutionDir)dependencies/windows/libtheora/include/;$(SolutionDir)dependencies/all/theoraplay/;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <SubSystem>Windows</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalLibraryDirectories>$(SolutionDir)dependencies/windows/SDL1/lib/$(PlatformTargetAsMSBuildArchitecture)/;$(SolutionDir)dependencies/windows/libvorbis/win32/VS2010/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libtheora/win32/VS2008/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libogg/win32/VS2015/$(Platform)/$(Configuration)/</AdditionalLibraryDirectories>
      <AdditionalDependencies>SDL.lib;SDLmain.lib;libogg.lib;libvorbis_static.lib;libvorbisfile_static.lib;libtheora_static.lib;Shell32.lib;Advapi32.lib</AdditionalDependencies>
    </Link>
    <PostBuildEvent>
      <Command>copy /Y "$(SolutionDir)dependencies\windows\SDL1\lib\$(PlatformTargetAsMSBuildArchitecture)\SDL.dll" "$(OutDir)" </Command>
      <Message>copying SDL1 library DLL to the build directory. build should fail if it's not found</Message>
    </PostBuildEvent>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <PrecompiledHeader>NotUsing</PrecompiledHeader>
      <WarningLevel>Level1</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <AdditionalIncludeDirectories>$(SolutionDir)dependencies/windows/SDL1/include/;$(SolutionDir)dependencies/windows/libogg/include/;$(SolutionDir)dependencies/windows/libvorbis/include/;$(SolutionDir)dependencies/windows/libtheora/include/;$(SolutionDir)dependencies/all/theoraplay/;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <SubSystem>Windows</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalLibraryDirectories>$(SolutionDir)dependencies/windows/SDL1/lib/$(PlatformTargetAsMSBuildArchitecture)/;$(SolutionDir)dependencies/windows/libvorbis/win32/VS2010/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libtheora/win32/VS2008/$(Platform)/$(Configuration)/;$(SolutionDir)dependencies/windows/libogg/win32/VS2015/$(Platform)/$(Configuration)/</AdditionalLibraryDirectories>
      <AdditionalDependencies>SDL.lib;SDLmain.lib;libogg.lib;libvorbis_static.lib;libvorbisfile_static.lib;libtheora_static.lib;Shell32.lib;Advapi32.lib</AdditionalDependencies>
    </Link>
    <PostBuildEvent>
      <Command>copy /Y "$(SolutionDir)dependencies\windows\SDL1\lib\$(PlatformTargetAsMSBuildArchitecture)\SDL.dll" "$(OutDir)" </Command>
      <Message>copying SDL1 library DLL to the build directory. build should fail if it's not found</Message>
    </PostBuildEvent>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="..\dependencies\all\theoraplay\theoraplay.c" />
    <ClCompile Include="Animation.cpp" />
    <ClCompile Include="Audio.cpp" />
    <ClCompile Include="Collision.cpp" />
    <ClCompile Include="Debug.cpp" />
    <ClCompile Include="Drawing.cpp" />
    <ClCompile Include="Scene3D.cpp" />
    <ClCompile Include="Ini.cpp" />
    <ClCompile Include="Input.cpp" />
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Math.cpp" />
    <ClCompile Include="Object.cpp" />
    <ClCompile Include="Palette.cpp" />
    <ClCompile Include="Player.cpp" />
    <ClCompile Include="Reader.cpp" />
    <ClCompile Include="RetroEngine.cpp" />
    <ClCompile Include="Scene.cpp" />
    <ClCompile Include="Script.cpp" />
    <ClCompile Include="Userdata.cpp" />
    <ClCompile Include="Sprite.cpp" />
    <ClCompile Include="String.cpp" />
    <ClCompile Include="Text.cpp" />
    <ClCompile Include="Video.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="..\dependencies\all\theoraplay\theoraplay.h" />
    <ClInclude Include="..\dependencies\all\theoraplay\theoraplay_cvtrgb.h" />
    <ClInclude Include="..\dependencies\windows\ValveFileVDF\vdf_parser.hpp" />
    <ClInclude Include="Animation.hpp" />
    <ClInclude Include="Audio.hpp" />
    <ClInclude Include="Collision.hpp" />
    <ClInclude Include="Debug.hpp" />
    <ClInclude Include="Drawing.hpp" />
    <ClInclude Include="Scene3D.hpp" />
    <ClInclude Include="Ini.hpp" />
    <ClInclude Include="Reader.hpp" />
    <ClInclude Include="resource.h" />
    <ClInclude Include="Sprite.hpp" />
    <ClInclude Include="Input.hpp" />
    <ClInclude Include="Math.hpp" />
    <ClInclude Include="Object.hpp" />
    <ClInclude Include="Palette.hpp" />
    <ClInclude Include="Player.hpp" />
    <ClInclude Include="RetroEngine.hpp" />
    <ClInclude Include="Scene.hpp" />
    <ClInclude Include="Script.hpp" />
    <ClInclude Include="String.hpp" />
    <ClInclude Include="Text.hpp" />
    <ClInclude Include="Userdata.hpp" />
    <ClInclude Include="Video.hpp" />
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="RSDKv3.rc" />
  </ItemGroup>
  <ItemGroup>
    <Image Include="win.ico" />
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>