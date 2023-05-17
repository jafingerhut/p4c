/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <boost/format.hpp>

#include "ir/declaration.h"
#include "ir/id.h"
#include "ir/ir.h"
#include "ir/vector.h"
#include "lib/cstring.h"
#include "lib/error.h"
#include "lib/error_catalog.h"
#include "lib/exceptions.h"

namespace IR {

cstring Annotation::getName() const {
    BUG_CHECK(name == IR::Annotation::nameAnnotation, "%1%: Only works on name annotations", this);
    if (needsParsing)
        // This can happen if this method is invoked before we have parsed
        // annotation bodies.
        return name;
    return getSingleString();
}

cstring Annotation::getSingleString() const {
    if (expr.size() != 1) {
        ::error(ErrorType::ERR_INVALID, "%1%: should contain a string", this);
        return "";
    }
    auto str = expr[0]->to<IR::StringLiteral>();
    if (str == nullptr) {
        ::error(ErrorType::ERR_INVALID, "%1%: should contain a string", this);
        return "";
    }
    return str->value;
}

cstring IDeclaration::externalName(cstring replace /* = cstring() */) const {
    if (!is<IAnnotated>()) return getName().toString();

    auto anno = getAnnotation(IR::Annotation::nameAnnotation);
    if (anno != nullptr) return anno->getName();
    if (replace) return replace;
    return getName().toString();
}

cstring IDeclaration::controlPlaneName(cstring replace /* = cstring() */) const {
    cstring name = externalName(replace);
    return name.startsWith(".") ? name.substr(1) : name;
}

}  // namespace IR
