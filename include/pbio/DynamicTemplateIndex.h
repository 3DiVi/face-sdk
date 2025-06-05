/**
    \file DynamicTemplateIndex.h
    \~English
    \brief DynamicTemplateIndex - Interface object for working with the template index.
    \~Russian
    \brief DynamicTemplateIndex - Интерфейсный объект для работы с индексом шаблонов.
*/

#ifndef __PBIO_API__PBIO__RESIZABLE_TEMPLATE_INDEX_H_
#define __PBIO_API__PBIO__RESIZABLE_TEMPLATE_INDEX_H_

#include "ComplexObject.h"
#include "Error.h"
#include "SmartPtr.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"
#include "Template.h"
#include "ContextTemplate.h"

namespace pbio
{
    /** \~English
        \brief Interface object for working with the template index.
        \~Russian
        \brief Интерфейсный объект для работы с индексом шаблонов.
    */
    class DynamicTemplateIndex
    {
    public:
        /** \~English
            \brief Alias for the type of a smart pointer to DynamicTemplateIndex.
            \~Russian
            \brief Псевдоним для типа умного указателя на DynamicTemplateIndex.
        */
        typedef LightSmartPtr<DynamicTemplateIndex>::tPtr Ptr;
        typedef LightSmartPtr<import::DllHandle>::tPtr DHPtr;

    public:
        /**
            \~English
            \brief
                Get a number of templates in the index.
                Thread-safe.

            \return
                Number of templates in the index.

            \~Russian
            \brief
                Получить количество шаблонов в индексе.
                Потокобезопасный.

            \return
                Количество шаблонов в индексе.
        */
        size_t size() const;

        size_t capacity() const;

        /**
            \~English
            \brief
                Get the uuid of the i-th template.
                Thread-safe.

            \param[in]  i
                Integer i: 0 <= i < size.

            \return
                The i-th template.

            \~Russian
            \brief
                Получить uuid i-го шаблона.
                Потокобезопасный.

            \param[in]  i
                Целое i: 0 <= i < size.

            \return
                i-й шаблон.
        */
        std::string getUUID(size_t index) const;

        pbio::ContextTemplate::Ptr at(const std::string& uuid) const;

        pbio::ContextTemplate::Ptr get(int64_t) const;

        void add(pbio::Template::Ptr templ, const std::string& uuid);

        void add(const std::vector<pbio::Template::Ptr>& templates, const std::vector<std::string>& uuids);

        void add(pbio::ContextTemplate::Ptr templ, const std::string& uuid);

        void add(const std::vector<pbio::ContextTemplate::Ptr>& templates, const std::vector<std::string>& uuids);

        void remove(const std::string& uuid);

        void remove(const std::vector<std::string>& uuids);

        void concat(DynamicTemplateIndex::Ptr otherIndex);

        void clear();

        /**
            \~English
            \brief
                Get a method name.
                Thread-safe.

            \return
                The name of the method created this index.

            \~Russian
            \brief
                Получить имя метода.
                Потокобезопасный.

            \return
                Имя метода, создавшего этот индекс.
        */
        std::string getMethodName() const;

        ~DynamicTemplateIndex();

    private:
        DynamicTemplateIndex(const DHPtr& dll_handle, void* impl, bool weak = false);

    private:
        const DHPtr _dll_handle;
        void* const _impl;
        int32_t refcounter4light_shared_ptr;
        bool weak;

        friend class object_with_ref_counter<DynamicTemplateIndex>;
        friend class Context;
    };
}

////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{
    inline DynamicTemplateIndex::DynamicTemplateIndex(const DHPtr& dll_handle, void* impl, bool weak) :
        _dll_handle(dll_handle),
        _impl(impl),
        weak(weak)
    {

    }

    inline std::string DynamicTemplateIndex::getMethodName() const
    {
        std::ostringstream name_stream;
        pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

        void* exception = nullptr;

        _dll_handle->DynamicTemplateIndex_getMethodName(
            _impl,
            &name_stream_wrap,
            pbio::stl_wraps::WrapOStream::write_func,
            &exception);

        checkException(exception, *_dll_handle);

        return name_stream.str();
    }

    inline size_t DynamicTemplateIndex::size() const
    {
        void* exception = nullptr;

        const size_t size = _dll_handle->DynamicTemplateIndex_size(
            _impl,
            &exception);

        checkException(exception, *_dll_handle);

        return size;
    }

    inline size_t DynamicTemplateIndex::capacity() const
    {
        void* exception = nullptr;

        size_t capacity = _dll_handle->DynamicTemplateIndex_capacity(
            _impl,
            &exception
        );

        checkException(exception, *_dll_handle);

        return capacity;
    }

    inline std::string DynamicTemplateIndex::getUUID(size_t index) const
    {
        void* exception = nullptr;
        std::ostringstream name_stream;
        pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

        _dll_handle->DynamicTemplateIndex_at_by_index(
            _impl,
            index,
            &name_stream_wrap,
            pbio::stl_wraps::WrapOStream::write_func,
            &exception);

        checkException(exception, *_dll_handle);

        return name_stream.str();
    }

    inline ContextTemplate::Ptr DynamicTemplateIndex::at(const std::string& uuid) const
    {
        void* exception = nullptr;

        void* implementation = _dll_handle->DynamicTemplateIndex_at_by_uuid(
            _impl,
            uuid.data(),
            &exception);

        checkException(exception, *_dll_handle);

        return ContextTemplate::Ptr::make(_dll_handle, implementation);
    }

    inline ContextTemplate::Ptr DynamicTemplateIndex::get(int64_t index) const
    {
        void* exception = nullptr;

        void* implementation = _dll_handle->DynamicTemplateIndex_get(
            _impl,
            index,
            &exception);

        checkException(exception, *_dll_handle);

        return ContextTemplate::Ptr::make(_dll_handle, implementation);
    }

    inline void DynamicTemplateIndex::add(pbio::Template::Ptr templ, const std::string& uuid)
    {
        void* exception = nullptr;

        _dll_handle->DynamicTemplateIndex_add_1(_impl, static_cast<pbio::facerec::TemplateImpl*>(templ->_impl), uuid.data(), &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::add(const std::vector<pbio::Template::Ptr>& templates, const std::vector<std::string>& uuids)
    {
        void* exception = nullptr;

        std::vector<pbio::facerec::TemplateImpl*> tempTemplates;
        std::vector<const char*> tempUuids;

        tempTemplates.reserve(templates.size());
        tempUuids.reserve(uuids.size());

        std::transform
        (
            templates.begin(), templates.end(), std::back_inserter(tempTemplates),
            [](pbio::Template::Ptr templ) { return reinterpret_cast<pbio::facerec::TemplateImpl*>(templ->_impl); }
        );
        std::transform(uuids.begin(), uuids.end(), std::back_inserter(tempUuids), [](const std::string& uuid) { return uuid.data(); });

        _dll_handle->DynamicTemplateIndex_add_2(_impl, tempTemplates.data(), tempUuids.data(), templates.size(), &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::add(pbio::ContextTemplate::Ptr templ, const std::string& uuid)
    {
        void* exception = nullptr;

        _dll_handle->DynamicTemplateIndex_add_3(_impl, templ->_impl, uuid.data(), &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::add(const std::vector<pbio::ContextTemplate::Ptr>& templates, const std::vector<std::string>& uuids)
    {
        void* exception = nullptr;

        std::vector<const void*> tempTemplates;
        std::vector<const char*> tempUuids;

        tempTemplates.reserve(templates.size());
        tempUuids.reserve(uuids.size());

        std::transform
        (
            templates.begin(), templates.end(), std::back_inserter(tempTemplates),
            [](pbio::ContextTemplate::Ptr templ) { return templ->_impl; }
        );
        std::transform(uuids.begin(), uuids.end(), std::back_inserter(tempUuids), [](const std::string& uuid) { return uuid.data(); });

        _dll_handle->DynamicTemplateIndex_add_4(_impl, tempTemplates.data(), tempUuids.data(), templates.size(), &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::remove(const std::string& uuid)
    {
        void* exception = nullptr;

        _dll_handle->DynamicTemplateIndex_remove_1(_impl, uuid.data(), &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::remove(const std::vector<std::string>& uuids)
    {
        void* exception = nullptr;
        std::vector<const char*> data;

        data.reserve(uuids.size());

        std::transform(uuids.begin(), uuids.end(), std::back_inserter(data), [](const std::string& uuid) { return uuid.data(); });

        _dll_handle->DynamicTemplateIndex_remove_2(_impl, data.data(), uuids.size(), &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::concat(DynamicTemplateIndex::Ptr otherIndex)
    {
        void* exception = nullptr;

        _dll_handle->DynamicTemplateIndex_concatenate(_impl, otherIndex->_impl, &exception);

        checkException(exception, *_dll_handle);
    }

    inline void DynamicTemplateIndex::clear()
    {
        void* exception = nullptr;

        _dll_handle->DynamicTemplateIndex_clear(_impl, &exception);

        checkException(exception, *_dll_handle);
    }

    inline DynamicTemplateIndex::~DynamicTemplateIndex()
    {
        if (_impl && !weak)
        {
            _dll_handle->DynamicTemplateIndex_destructor(_impl);
        }
    }
}
#endif
