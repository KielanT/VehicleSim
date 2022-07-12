#pragma once

#define	SAFE_RELEASE(x)	if(x){ x->release(); x = NULL;	} // Physx macro to release pointers
#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))