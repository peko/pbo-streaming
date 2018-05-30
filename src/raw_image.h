#pragma once

// #include "raw_image_abstract.h"
#include <dlib/algs.h>
#include <dlib/pixel.h>
#include <dlib/matrix/matrix_mat.h>
#include <dlib/image_processing/generic_image.h>

namespace dlib {

    template <typename pixel_type>
    class raw_image {
        
    public:
        typedef pixel_type type;
        typedef default_memory_manager mem_manager_type;
        long w;
        long h;
        long c;
        void* d;
        
        raw_image() : w(0), h(0), c(0), d(0) { }
        raw_image(long w, long h, long c, void* d) : w(w), h(h), c(c), d(d) { }
        size_t size () const { return static_cast<size_t>(w*h); }
	};

// ----------------------------------------------------------------------------------------

//    template <typename T>
//    const matrix_op<op_array2d_to_mat<raw_image<T> > > mat (
//        const raw_image<T>& m 
//    )
//    {
//        typedef op_array2d_to_mat<raw_image<T> > op;
//        return matrix_op<op>(op(m));
//    }

// ----------------------------------------------------------------------------------------

// Define the global functions that make raw_image a proper "generic image" according to
// ../image_processing/generic_image.h

    template <typename T>
    struct image_traits<raw_image<T> > {
        typedef T pixel_type;
    };

    template <typename T>
    inline long num_rows( const raw_image<T>& img) { 
        return img.h; 
    }

    template <typename T>
    inline long num_columns( const raw_image<T>& img) { 
        return img.w; 
    }

    template <typename T>
    inline void* image_data(raw_image<T>& img) {
        return img.d;
    }

    template <typename T>
    inline const void* image_data(const raw_image<T>& img) {
        return img.d;
    }

    template <typename T>
    inline long width_step(const raw_image<T>& img) { 
        return img.w*img.c; 
    }

}

