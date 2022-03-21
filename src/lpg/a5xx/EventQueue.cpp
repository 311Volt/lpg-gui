#include <lpg/a5xx/EventQueue.hpp>

#include <stdexcept>

al::EventQueue::EventQueue()
{
	alPtr = al_create_event_queue();
	if(!alPtr) {
		throw std::runtime_error("Failed to create Allegro event queue");
	}
}

al::EventQueue::~EventQueue()
{
	al_destroy_event_queue(alPtr);
}


void al::EventQueue::registerSource(ALLEGRO_EVENT_SOURCE* src)
{
	al_register_event_source(alPtr, src);
}

bool al::EventQueue::empty()
{
	bool ret = al_is_event_queue_empty(alPtr);
	return ret;
}

ALLEGRO_EVENT al::EventQueue::pop()
{
	ALLEGRO_EVENT ret;
	al_get_next_event(alPtr, &ret);
	return ret;
}

ALLEGRO_EVENT al::EventQueue::peek()
{
	ALLEGRO_EVENT ret;
	al_peek_next_event(alPtr, &ret);
	return ret;
}

bool al::EventQueue::drop()
{
	return al_drop_next_event(alPtr);
}

void al::EventQueue::clear()
{
	al_flush_event_queue(alPtr);
}

ALLEGRO_EVENT_QUEUE* al::EventQueue::get()
{
	return alPtr;
}
