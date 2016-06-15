
#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>

enum CarType
{
	UnknownType = 0,
	CompactCar,
	Automobile,
	AutomobileSedan,
	TruckSmall,
	Truck2T,
	Truck4T,
	DumpSmall,
	DumpMedium,
	DumpLarge,
	Microbus,
	BusinessBusSmall,
	BusinessBusLarge		
};

enum ImageIndex
{
	CurImage = 0,
	PreImage1,
	PreImage2
};