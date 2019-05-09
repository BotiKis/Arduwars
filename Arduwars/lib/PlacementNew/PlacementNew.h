#pragma once

//
// Copyright (C) 2018 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stddef.h>

//
// See https://en.cppreference.com/w/cpp/language/new#Placement_new
//
// > Such allocation functions are known as "placement new",
// > after the standard allocation function void* operator new(std::size_t, void*),
// > which simply returns its second argument unchanged"
//

inline void * operator new (size_t size, void * pointer)
{
	static_cast<void>(size);
	return pointer;
}
