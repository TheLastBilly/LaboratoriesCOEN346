#include "thread_controller.hpp"

void thread_controller::create_thread()
{
    if (this->thread.joinable())
        return;

    this->alive = true;
    this->status = status_t::IDLE;
    this->thread = std::thread([&]() {
        while (this->alive)
        {
            // Wait until the condition variable has been set to not
            // IDLE
            std::unique_lock<std::mutex> lck(this->mtx);
            while (this->status == status_t::IDLE)
                cv.wait(lck);
            lck.unlock();

            switch (this->status)
            {
            case status_t::IDLE:
                break;
            case status_t::TERMINATED:
                this->alive = false;
                break;
            case status_t::RUNNING:
                this->cycle();
                break;
            }
        }
        this->status = status_t::TERMINATED;
    });
}
std::thread &thread_controller::get_thread()
{
    return this->thread;
}

thread_controller::status_t thread_controller::get_status() const
{
    return this->status;
}
void thread_controller::set_status(status_t status)
{
    std::unique_lock<std::mutex> lck(this->mtx);
    this->status = status;
    cv.notify_all();
}

void thread_controller::run()
{
    this->set_status(status_t::RUNNING);
}
void thread_controller::pause()
{
    this->set_status(status_t::IDLE);
}
void thread_controller::terminate()
{
    this->set_status(status_t::TERMINATED);
    if (this->thread.joinable())
        this->thread.join();
}