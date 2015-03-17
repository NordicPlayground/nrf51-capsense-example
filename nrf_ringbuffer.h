#ifndef __QLIB_RINGBUFFER_H__
#define __QLIB_RINGBUFFER_H__

#include "nrf.h"

#define NRF_RINGBUFFER_SIZE 32

typedef struct 
{
    uint32_t in, out, size;
    uint8_t data[NRF_RINGBUFFER_SIZE];
}ringbuffer8_t;

static __INLINE void nrf_ringbuffer8_init(volatile ringbuffer8_t *ringbuf)
{
    ringbuf->in = ringbuf->out = ringbuf->size = 0;
}

static __INLINE void nrf_ringbuffer8_put(volatile ringbuffer8_t *ringbuf, uint8_t newdata)
{
    if(ringbuf->size < NRF_RINGBUFFER_SIZE)
    {
        ringbuf->size++;
        ringbuf->data[ringbuf->in++] = newdata;
        if(ringbuf->in >= NRF_RINGBUFFER_SIZE)
        {
            ringbuf->in = 0;
        }
    }
}

static __INLINE uint8_t nrf_ringbuffer8_get(volatile ringbuffer8_t *ringbuf)
{
    uint8_t retval;
    if(ringbuf->size > 0) 
    {
        ringbuf->size--;
        retval = ringbuf->data[ringbuf->out++];
        if(ringbuf->out >= NRF_RINGBUFFER_SIZE)
        {
            ringbuf->out = 0;
        }
        return retval;
    }
    return 0;
}

static __INLINE uint8_t nrf_ringbuffer8_empty(volatile ringbuffer8_t *ringbuf)
{
    if(ringbuf->size == 0) return 1;
    else return 0;   
}

static __INLINE uint8_t nrf_ringbuffer8_full(volatile ringbuffer8_t *ringbuf)
{
    if(ringbuf->size >= NRF_RINGBUFFER_SIZE) return 1;
    else return 0;
}

#endif
