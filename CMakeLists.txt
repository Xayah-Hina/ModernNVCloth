cmake_minimum_required(VERSION 3.24)
project(ModernNVCloth VERSION 1.0.0 DESCRIPTION "Proceed to Modern NV Cloth" LANGUAGES CXX)

add_executable(ModernNVCloth)
set_target_properties(ModernNVCloth PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_sources(ModernNVCloth
        PRIVATE
        main.cpp

        nv_cloth/include/NvCloth/Allocator.h
        nv_cloth/include/NvCloth/Callbacks.h
        nv_cloth/include/NvCloth/Cloth.h
        nv_cloth/include/NvCloth/DxContextManagerCallback.h
        nv_cloth/include/NvCloth/Fabric.h
        nv_cloth/include/NvCloth/Factory.h
        nv_cloth/include/NvCloth/PhaseConfig.h
        nv_cloth/include/NvCloth/Range.h
        nv_cloth/include/NvCloth/Solver.h

        nv_cloth/include/NvCloth/ps/Ps.h
        nv_cloth/include/NvCloth/ps/PsAtomic.h
        nv_cloth/include/NvCloth/ps/PsAlignedMalloc.h
        nv_cloth/include/NvCloth/ps/PsAllocator.h
        nv_cloth/include/NvCloth/ps/PsAllocator.cpp
        nv_cloth/include/NvCloth/ps/PsArray.h
        nv_cloth/include/NvCloth/ps/PsBasicTemplates.h
        nv_cloth/include/NvCloth/ps/PsBitUtils.h
        nv_cloth/include/NvCloth/ps/PsHash.h
        nv_cloth/include/NvCloth/ps/PsHashInternals.h
        nv_cloth/include/NvCloth/ps/PsHashMap.h
        nv_cloth/include/NvCloth/ps/PsIntrinsics.h
        nv_cloth/include/NvCloth/ps/PsUserAllocated.h
        nv_cloth/include/NvCloth/ps/PsMathUtils.h

        nv_cloth/src/NvCloth/Allocator.cpp
        nv_cloth/src/NvCloth/BoundingBox.h
        nv_cloth/src/NvCloth/Callbacks.cpp
        nv_cloth/src/NvCloth/ClothBase.h
        nv_cloth/src/NvCloth/ClothClone.h
        nv_cloth/src/NvCloth/ClothImpl.h
        nv_cloth/src/NvCloth/Factory.cpp
        nv_cloth/src/NvCloth/IndexPair.h
        nv_cloth/src/NvCloth/IterationState.h
        nv_cloth/src/NvCloth/MovingAverage.h
        nv_cloth/src/NvCloth/PhaseConfig.cpp
        nv_cloth/src/NvCloth/PointInterpolator.h
        nv_cloth/src/NvCloth/Simd.h
        nv_cloth/src/NvCloth/StackAllocator.h
        nv_cloth/src/NvCloth/SwCloth.cpp
        nv_cloth/src/NvCloth/SwCloth.h
        nv_cloth/src/NvCloth/SwClothData.cpp
        nv_cloth/src/NvCloth/SwClothData.h
        nv_cloth/src/NvCloth/SwCollision.cpp
        nv_cloth/src/NvCloth/SwCollision.h
        nv_cloth/src/NvCloth/SwCollisionHelpers.h
        nv_cloth/src/NvCloth/SwFabric.cpp
        nv_cloth/src/NvCloth/SwFabric.h
        nv_cloth/src/NvCloth/SwFactory.cpp
        nv_cloth/src/NvCloth/SwFactory.h
        nv_cloth/src/NvCloth/SwInterCollision.cpp
        nv_cloth/src/NvCloth/SwInterCollision.h
        nv_cloth/src/NvCloth/SwSelfCollision.cpp
        nv_cloth/src/NvCloth/SwSelfCollision.h
        nv_cloth/src/NvCloth/SwSolver.cpp
        nv_cloth/src/NvCloth/SwSolver.h
        nv_cloth/src/NvCloth/SwSolverKernel.cpp
        nv_cloth/src/NvCloth/SwSolverKernel.h
        nv_cloth/src/NvCloth/TripletScheduler.cpp
        nv_cloth/src/NvCloth/TripletScheduler.h
        nv_cloth/src/NvCloth/Vec4T.h

        nv_cloth/src/NvCloth/NvSimd/NvSimd4f.h
        nv_cloth/src/NvCloth/NvSimd/NvSimd4i.h
        nv_cloth/src/NvCloth/NvSimd/NvSimdTypes.h
        nv_cloth/src/NvCloth/NvSimd/neon/NvNeonSimd4f.h
        nv_cloth/src/NvCloth/NvSimd/neon/NvNeonSimd4i.h
        nv_cloth/src/NvCloth/NvSimd/neon/NvNeonSimdTypes.h
        nv_cloth/src/NvCloth/NvSimd/scalar/NvScalarSimd4f.h
        nv_cloth/src/NvCloth/NvSimd/scalar/NvScalarSimd4i.h
        nv_cloth/src/NvCloth/NvSimd/scalar/NvScalarSimdTypes.h
        nv_cloth/src/NvCloth/NvSimd/sse2/NvSse2Simd4f.h
        nv_cloth/src/NvCloth/NvSimd/sse2/NvSse2Simd4i.h
        nv_cloth/src/NvCloth/NvSimd/sse2/NvSse2SimdTypes.h
        nv_cloth/src/NvCloth/scalar/SwCollisionHelpers.h
        nv_cloth/src/NvCloth/sse2/SwCollisionHelpers.h
        nv_cloth/src/NvCloth/sse2/SwSolveConstraints.h

        nv_cloth/src/NvCloth/ps/PsAlloca.h
        nv_cloth/src/NvCloth/ps/PsFPU.h
        nv_cloth/src/NvCloth/ps/PsSort.h
        nv_cloth/src/NvCloth/ps/PsSortInternals.h
        nv_cloth/src/NvCloth/ps/PsUtilities.h
        nv_cloth/src/NvCloth/ps/PxIntrinsics.h

        nv_cloth/include/NvClothExt/ClothFabricCooker.h
        nv_cloth/include/NvClothExt/ClothMeshDesc.h
        nv_cloth/include/NvClothExt/ClothMeshQuadifier.h
        nv_cloth/include/NvClothExt/ClothTetherCooker.h
        nv_cloth/src/NvClothExt/ClothFabricCooker.cpp
        nv_cloth/src/NvClothExt/ClothGeodesicTetherCooker.cpp
        nv_cloth/src/NvClothExt/ClothMeshQuadifier.cpp
        nv_cloth/src/NvClothExt/ClothSimpleTetherCooker.cpp

        nv_cloth/src/NvCloth/avx/SwSolveConstraints.cpp

        # For Windows Only
        nv_cloth/src/NvCloth/ps/windows/PsWindowsAtomic.cpp
        nv_cloth/src/NvCloth/ps/windows/PsWindowsFPU.h
        nv_cloth/src/NvCloth/ps/windows/PsWindowsInclude.h
        )
set_source_files_properties(nv_cloth/src/NvCloth/avx/SwSolveConstraints.cpp PROPERTIES COMPILE_FLAGS "/arch:AVX")
target_include_directories(ModernNVCloth
        PUBLIC
        nv_cloth/include
        nv_cloth/src/NvCloth
        )
target_compile_definitions(ModernNVCloth
        PUBLIC
        NV_CLOTH_ENABLE_DX11=0
        NV_CLOTH_ENABLE_CUDA=0
        NV_CLOTH_IMPORT=PX_DLL_EXPORT
        )