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

#ifndef BACKENDS_BMV2_COMMON_ACTION_H_
#define BACKENDS_BMV2_COMMON_ACTION_H_

#include "helpers.h"
#include "ir/ir.h"

namespace P4::BMV2 {

class ActionConverter : public Inspector {
    ConversionContext *ctxt;

    void convertActionBodyTop(const IR::Vector<IR::StatOrDecl> *body, Util::JsonArray *result);
    void convertActionBody(const IR::Vector<IR::StatOrDecl> *body,
                           Util::JsonArray *result,
                           bool inConditional,
                           int labelIdEndOfAction,
                           std::map<int,int> *labelIdToJumpOffset,
                           int *numLabels,
                           std::map<int,int> *offsetToTargetLabelId,
                           std::map<int,Util::JsonArray*> *offsetToJumpParams);
    void convertActionParams(const IR::ParameterList *parameters, Util::JsonArray *params);
    cstring jsonAssignment(const IR::Type *type);
    void postorder(const IR::P4Action *action) override;

 public:
    const bool emitExterns;
    explicit ActionConverter(ConversionContext *ctxt, const bool &emitExterns_)
        : ctxt(ctxt), emitExterns(emitExterns_) {
        setName("ConvertActions");
    }
};

}  // namespace P4::BMV2

#endif /* BACKENDS_BMV2_COMMON_ACTION_H_ */
