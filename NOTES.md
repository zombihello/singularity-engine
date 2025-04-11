# StudioAPIVk
* Descriptor sets
	1. At now descriptor sets always recreates when anyone has been updated. To reduce they amount we can implement descriptor set cache for one frame,
	but it can be in some cases too expensive by memory
* Transfer ownership barriers in CStudioAPIBufferVk::UpdateData and CStudioAPITextureVk::UpdateData
	1. Maybe no need do transfer ownership in those functions and it must do user on outside