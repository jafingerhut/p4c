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

#ifndef IR_VECTOR_H_
#define IR_VECTOR_H_

#include "ir/node.h"
#include "lib/enumerator.h"
#include "lib/indent.h"
#include "lib/null.h"
#include "lib/safe_vector.h"

namespace P4 {
class JSONLoader;
}  // namespace P4

namespace P4::IR {

// Specialization of vector which
// - only stores const IR::Node* objects inside (T should derive from Node)
// - inherits from IR::Node itself
class VectorBase : public Node {
 public:
    typedef const Node *const *iterator;
    virtual iterator VectorBase_begin() const = 0;
    virtual iterator VectorBase_end() const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    iterator begin() const { return VectorBase_begin(); }
    iterator end() const { return VectorBase_end(); }
    VectorBase() = default;
    VectorBase(const VectorBase &) = default;
    VectorBase(VectorBase &&) = default;
    VectorBase &operator=(const VectorBase &) = default;
    VectorBase &operator=(VectorBase &&) = default;

 protected:
    explicit VectorBase(JSONLoader &json) : Node(json) {}

    DECLARE_TYPEINFO_WITH_TYPEID(VectorBase, NodeKind::VectorBase, Node);
};

// This class should only be used in the IR.
// User-level code should use regular std::vector
template <class T>
class Vector : public VectorBase {
    safe_vector<const T *> vec;

 public:
    typedef const T *value_type;
    Vector() = default;
    Vector(const Vector &) = default;
    Vector(Vector &&) = default;
    explicit Vector(JSONLoader &json);
    Vector &operator=(const Vector &) = default;
    Vector &operator=(Vector &&) = default;
    explicit Vector(const T *a) { vec.emplace_back(a); }
    explicit Vector(const safe_vector<const T *> &a) { vec.insert(vec.end(), a.begin(), a.end()); }
    Vector(std::initializer_list<const T *> a) : vec(a) {}
    template <class InputIt>
    Vector(InputIt first, InputIt last) : vec(first, last) {}
    Vector(Util::Enumerator<const T *> *e)  // NOLINT(runtime/explicit)
        : vec(e->begin(), e->end()) {}
    static Vector<T> *fromJSON(JSONLoader &json);

    using iterator = typename safe_vector<const T *>::iterator;
    using const_iterator = typename safe_vector<const T *>::const_iterator;

    iterator begin() { return vec.begin(); }
    const_iterator begin() const { return vec.begin(); }
    VectorBase::iterator VectorBase_begin() const override {
        /* DANGER -- works as long as IR::Node is the first ultimate base class of T */
        return reinterpret_cast<VectorBase::iterator>(&vec[0]);
    }
    iterator end() { return vec.end(); }
    const_iterator end() const { return vec.end(); }
    VectorBase::iterator VectorBase_end() const override {
        /* DANGER -- works as long as IR::Node is the first ultimate base class of T */
        return reinterpret_cast<VectorBase::iterator>(&vec[0] + vec.size());
    }
    std::reverse_iterator<iterator> rbegin() { return vec.rbegin(); }
    std::reverse_iterator<const_iterator> rbegin() const { return vec.rbegin(); }
    std::reverse_iterator<iterator> rend() { return vec.rend(); }
    std::reverse_iterator<const_iterator> rend() const { return vec.rend(); }
    size_t size() const override { return vec.size(); }
    void resize(size_t sz) { vec.resize(sz); }
    bool empty() const override { return vec.empty(); }
    const T *const &front() const { return vec.front(); }
    const T *&front() { return vec.front(); }
    void clear() { vec.clear(); }
    iterator erase(iterator i) { return vec.erase(i); }
    iterator erase(iterator s, iterator e) { return vec.erase(s, e); }
    template <typename ForwardIter>
    iterator insert(iterator i, ForwardIter b, ForwardIter e) {
        return vec.insert(i, b, e);
    }

    template <typename Container>
    iterator append(const Container &toAppend) {
        return insert(end(), toAppend.begin(), toAppend.end());
    }
    template <typename Container>
    iterator prepend(const Container &toAppend) {
        return insert(begin(), toAppend.begin(), toAppend.end());
    }

    /**
     * Appends the provided node or vector of nodes to the end of this Vector.
     *
     * @param item  A node to append; if this is a Vector, its contents will be
     *              appended.
     */
    void pushBackOrAppend(const IR::Node *item) {
        if (item == nullptr) return;
        if (auto *itemAsVector = item->to<IR::Vector<T>>()) {
            append(*itemAsVector);
            return;
        }
        BUG_CHECK(item->is<T>(), "Unexpected vector element: %1%", item);
        push_back(item->to<T>());
    }

    iterator insert(iterator i, const T *v) { return vec.insert(i, v); }
    iterator insert(iterator i, size_t n, const T *v) { return vec.insert(i, n, v); }

    const T *const &operator[](size_t idx) const { return vec[idx]; }
    const T *&operator[](size_t idx) { return vec[idx]; }
    const T *const &at(size_t idx) const { return vec.at(idx); }
    const T *&at(size_t idx) { return vec.at(idx); }
    template <class... Args>
    void emplace_back(Args &&...args) {
        vec.emplace_back(new T(std::forward<Args>(args)...));
    }
    void push_back(T *a) { vec.push_back(a); }
    void push_back(const T *a) { vec.push_back(a); }
    void pop_back() { vec.pop_back(); }
    const T *const &back() const { return vec.back(); }
    const T *&back() { return vec.back(); }
    template <class U>
    void push_back(U &a) {
        vec.push_back(a);
    }
    void check_null() const {
        for (auto e : vec) CHECK_NULL(e);
    }

    IRNODE_SUBCLASS(Vector)
    IRNODE_DECLARE_APPLY_OVERLOAD(Vector)
    bool operator==(const Node &a) const override { return a == *this; }
    bool operator==(const Vector &a) const override { return vec == a.vec; }
    /* DANGER -- if you get an error on the above line
     *       operator== ... marked ‘override’, but does not override
     * that mean you're trying to create an instantiation of IR::Vector that
     * does not appear anywhere in any .def file, which won't work.
     * To make double-dispatch comparisons work, the IR generator must know
     * about ALL instantiations of IR class templates, which it does by scanning
     * all the .def files for instantiations.  This could in theory be fixed by
     * having the IR generator scan all C++ header and source files for
     * instantiations, but that is currently not done.
     *
     * To avoid this problem, you need to have your code ONLY use instantiations
     * of IR::Vector that appear somewhere in a .def file -- you can usually make
     * it work by using an instantiation with an (abstract) base class rather
     * than a concrete class, as most of those appear in .def files. */
    bool equiv(const Node &a_) const override {
        if (static_cast<const Node *>(this) == &a_) return true;
        if (this->typeId() != a_.typeId()) return false;
        auto &a = static_cast<const Vector<T> &>(a_);
        if (size() != a.size()) return false;
        auto it = a.begin();
        for (auto *el : *this)
            if (!el->equiv(**it++)) return false;
        return true;
    }
    cstring node_type_name() const override { return "Vector<" + T::static_type_name() + ">"; }
    static cstring static_type_name() { return "Vector<" + T::static_type_name() + ">"; }
    void visit_children(Visitor &v, const char *name) override;
    void visit_children(Visitor &v, const char *name) const override;
    virtual void parallel_visit_children(Visitor &v, const char *name = nullptr);
    virtual void parallel_visit_children(Visitor &v, const char *name = nullptr) const;
    void toJSON(JSONGenerator &json) const override;
    Util::Enumerator<const T *> *getEnumerator() const { return Util::enumerate(vec); }
    template <typename S>
    Util::Enumerator<const S *> *only() const {
        return getEnumerator()->template as<const S *>()->where(
            [](const T *d) { return d != nullptr; });
    }
    template <class Filter>
    auto where(Filter f) const {
        return getEnumerator()->where(std::move(f));
    }

    void dbprint(std::ostream &out) const override {
        out << "{" << IndentCtl::indent;
        for (auto p : *this) {
            out << " " << p;
        }
        out << IndentCtl::unindent << " }";
    }

    DECLARE_TYPEINFO_WITH_DISCRIMINATOR(Vector<T>, NodeDiscriminator::VectorT, T, VectorBase);
};

template <class T, class U>
const T *get(const IR::Vector<T> &vec, U name) {
    for (auto el : vec)
        if (el->name == name) return el;
    return nullptr;
}
template <class T, class U>
const T *get(const IR::Vector<T> *vec, U name) {
    if (vec)
        for (auto el : *vec)
            if (el->name == name) return el;
    return nullptr;
}

}  // namespace P4::IR

#endif /* IR_VECTOR_H_ */
