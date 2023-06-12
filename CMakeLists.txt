cmake_minimum_required(VERSION 3.24)
project(ModernNVCloth VERSION 1.0.0 DESCRIPTION "Proceed to Modern NV Cloth" LANGUAGES CXX)

add_library(NVCloth STATIC)
set_target_properties(NVCloth PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_sources(NVCloth
        PRIVATE
        include/NvCloth/Allocator.h
        include/NvCloth/Callbacks.h
        include/NvCloth/Cloth.h
        include/NvCloth/DxContextManagerCallback.h
        include/NvCloth/Fabric.h
        include/NvCloth/Factory.h
        include/NvCloth/PhaseConfig.h
        include/NvCloth/Range.h
        include/NvCloth/Solver.h

        include/NvCloth/ps/Ps.h
        include/NvCloth/ps/PsAtomic.h
        include/NvCloth/ps/PsAlignedMalloc.h
        include/NvCloth/ps/PsAllocator.h
        include/NvCloth/ps/PsAllocator.cpp
        include/NvCloth/ps/PsArray.h
        include/NvCloth/ps/PsBasicTemplates.h
        include/NvCloth/ps/PsBitUtils.h
        include/NvCloth/ps/PsHash.h
        include/NvCloth/ps/PsHashInternals.h
        include/NvCloth/ps/PsHashMap.h
        include/NvCloth/ps/PsIntrinsics.h
        include/NvCloth/ps/PsUserAllocated.h
        include/NvCloth/ps/PsMathUtils.h

        src/NvCloth/Allocator.cpp
        src/NvCloth/BoundingBox.h
        src/NvCloth/Callbacks.cpp
        src/NvCloth/ClothBase.h
        src/NvCloth/ClothClone.h
        src/NvCloth/ClothImpl.h
        src/NvCloth/Factory.cpp
        src/NvCloth/IndexPair.h
        src/NvCloth/IterationState.h
        src/NvCloth/MovingAverage.h
        src/NvCloth/PhaseConfig.cpp
        src/NvCloth/PointInterpolator.h
        src/NvCloth/Simd.h
        src/NvCloth/StackAllocator.h
        src/NvCloth/SwCloth.cpp
        src/NvCloth/SwCloth.h
        src/NvCloth/SwClothData.cpp
        src/NvCloth/SwClothData.h
        src/NvCloth/SwCollision.cpp
        src/NvCloth/SwCollision.h
        src/NvCloth/SwCollisionHelpers.h
        src/NvCloth/SwFabric.cpp
        src/NvCloth/SwFabric.h
        src/NvCloth/SwFactory.cpp
        src/NvCloth/SwFactory.h
        src/NvCloth/SwInterCollision.cpp
        src/NvCloth/SwInterCollision.h
        src/NvCloth/SwSelfCollision.cpp
        src/NvCloth/SwSelfCollision.h
        src/NvCloth/SwSolver.cpp
        src/NvCloth/SwSolver.h
        src/NvCloth/SwSolverKernel.cpp
        src/NvCloth/SwSolverKernel.h
        src/NvCloth/TripletScheduler.cpp
        src/NvCloth/TripletScheduler.h
        src/NvCloth/Vec4T.h

        src/NvCloth/NvSimd/NvSimd4f.h
        src/NvCloth/NvSimd/NvSimd4i.h
        src/NvCloth/NvSimd/NvSimdTypes.h
        src/NvCloth/NvSimd/neon/NvNeonSimd4f.h
        src/NvCloth/NvSimd/neon/NvNeonSimd4i.h
        src/NvCloth/NvSimd/neon/NvNeonSimdTypes.h
        src/NvCloth/NvSimd/scalar/NvScalarSimd4f.h
        src/NvCloth/NvSimd/scalar/NvScalarSimd4i.h
        src/NvCloth/NvSimd/scalar/NvScalarSimdTypes.h
        src/NvCloth/NvSimd/sse2/NvSse2Simd4f.h
        src/NvCloth/NvSimd/sse2/NvSse2Simd4i.h
        src/NvCloth/NvSimd/sse2/NvSse2SimdTypes.h
        src/NvCloth/scalar/SwCollisionHelpers.h
        src/NvCloth/sse2/SwCollisionHelpers.h
        src/NvCloth/sse2/SwSolveConstraints.h

        src/NvCloth/ps/PsAlloca.h
        src/NvCloth/ps/PsFPU.h
        src/NvCloth/ps/PsSort.h
        src/NvCloth/ps/PsSortInternals.h
        src/NvCloth/ps/PsUtilities.h
        src/NvCloth/ps/PxIntrinsics.h

        include/NvClothExt/ClothFabricCooker.h
        include/NvClothExt/ClothMeshDesc.h
        include/NvClothExt/ClothMeshQuadifier.h
        include/NvClothExt/ClothTetherCooker.h
        src/NvClothExt/ClothFabricCooker.cpp
        src/NvClothExt/ClothGeodesicTetherCooker.cpp
        src/NvClothExt/ClothMeshQuadifier.cpp
        src/NvClothExt/ClothSimpleTetherCooker.cpp

        src/NvCloth/avx/SwSolveConstraints.cpp

        # For Windows Only
        src/NvCloth/ps/windows/PsWindowsAtomic.cpp
        src/NvCloth/ps/windows/PsWindowsFPU.h
        src/NvCloth/ps/windows/PsWindowsInclude.h
        )
set_source_files_properties(src/NvCloth/avx/SwSolveConstraints.cpp PROPERTIES COMPILE_FLAGS "/arch:AVX")
target_include_directories(NVCloth
        PUBLIC
        include
        src/NvCloth
        )
target_compile_definitions(NVCloth
        PUBLIC
        NV_CLOTH_ENABLE_DX11=0
        NV_CLOTH_ENABLE_CUDA=0
        NV_CLOTH_IMPORT=PX_DLL_EXPORT
        )