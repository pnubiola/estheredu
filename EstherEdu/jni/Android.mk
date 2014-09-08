# This file is part of EstherEdu.
#
#    EstherEdu is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    EstherEdu is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with EstherEdu.  If not, see <http://www.gnu.org/licenses/>.
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := lib/arm/libcrypto.a

include $(PREBUILD_STATIC_LIBRARIES)

include $(CLEAR_VARS)

LOCAL_MODULE := libssl
LOCAL_SRC_FILES := lib/arm/libssl.a

include $(PREBUILD_STATIC_LIBRARIES)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := src/httpfunc.c \
				   src/sslfunc.c \
				   src/EstherEduProvider.c 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CFLAGS += -std=c99

LOCAL_LDLIBS =  -lz

LOCAL_STATIC_LIBRARIES := libcrypto \
						  libssl

$(warning $(TARGET_ARCH))
$(warning $(TARGET_ARCH_ABI))
						  
$(warning $(LOCAL_STATIC_LIBRARIES))
						  
LOCAL_MODULE := estheredu

include $(BUILD_SHARED_LIBRARY)
