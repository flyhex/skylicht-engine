if (BUILD_EXAMPLE)
	subdirs (Samples/HelloWorld)
	subdirs (Samples/LuckyDraw)
	subdirs (Samples/SkinnedMesh)
	subdirs (Samples/Sponza)
	subdirs (Samples/TankScene)
	
	if (NOT BUILD_EMSCRIPTEN)
		subdirs (Samples/LightmapUV)
	endif()
endif()