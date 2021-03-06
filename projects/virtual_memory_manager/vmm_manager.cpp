#include "vmm.hpp"

namespace vmm
{
    vmm_manager::vmm_manager(size_t page_count, std::string file_path) : file_path(file_path), page_count(page_count), file(file_path)
    {
        this->pages = new page_t[page_count];
    }
    vmm_manager::~vmm_manager()
    {
        delete[] this->pages;
    }

    void vmm_manager::set_logger_callback(logger_callback_t c)
    {
        this->logger_callback = c;
    }
    void vmm_manager::set_timer_callback(timer_callback_t c)
    {
        this->timer_callback = c;
    }

    void vmm_manager::log(std::string msg)
    {
        if (this->logger_callback)
            this->logger_callback(msg);
    }
    size_t vmm_manager::get_time()
    {
        if (this->timer_callback)
            return this->timer_callback();

        return 0;
    }

    void vmm_manager::store(std::string id, uint value)
    {
        for (size_t i = 0; i < page_count; i++)
        {
            if (pages[i].available)
            {
                pages[i].last_access_time = this->get_time();
                pages[i].id = id;
                pages[i].val = value;
                pages[i].available = false;
                return;
            }
        }

        this->file.read();
        page_t p = {};
        p.val = value;
        p.id = id;
        p.last_access_time = this->get_time();
        this->file.add_page(p);
        this->file.write();
    }

    bool vmm_manager::release(std::string id)
    {
        for (size_t i = 0; i < page_count; i++)
        {
            if (pages[i].id == id)
            {
                pages[i].available = true;
                pages[i].id = "";
                pages[i].val = 0;
                return true;
            }
        }

        return false;
    }

    long int vmm_manager::lookup(std::string id)
    {
        for (size_t i = 0; i < page_count; i++)
        {
            if (pages[i].id == id)
            {
                pages[i].last_access_time = this->get_time();
                return pages[i].val;
            }
        }

        file.read();
        vmm_file::pages_t &p_pages = file.get_pages();
        for (size_t i = 0; i < p_pages.size(); i++)
        {
            if (p_pages[i].id == id)
            {
                size_t oldest = 0;
                size_t oldest_time = ~((size_t)0);
                for (size_t s = 0; s < page_count; s++)
                {
                    if (pages[s].last_access_time < oldest_time)
                    {
                        oldest_time = pages[s].last_access_time;
                        oldest = i;
                    }
                }

                page_t p = pages[oldest];
                this->log("Memory Manager, SWAP: Variable " + p_pages[i].id + " with Variable " + p.id);

                pages[oldest] = p_pages[i];
                pages[oldest].available = false;
                p_pages[i] = p;
                p_pages[i].last_access_time = this->get_time();

                file.write();
                return p_pages[i].val;
            }
        }

        return -1;
    }
}