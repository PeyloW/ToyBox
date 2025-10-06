//
//  concepts.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-05.
//

#ifndef concepts_hpp
#define concepts_hpp

#include "type_traits.hpp"

namespace toybox {
    
    template<typename A, typename B>
    concept same_as = is_same<A, B>::value;
    
    template<typename F, typename... Args>
    concept invocable = requires(F&& f, Args&&... args) {
        { f(static_cast<Args&&>(args)...) };
    };
    
    template<typename R, typename F, typename... Args>
    concept invocable_r = requires(F&& f, Args&&... args) {
        { f(static_cast<Args&&>(args)...) } -> same_as<R>;
    };
    
    template<typename I>
    concept incrementable = requires(I i) {
        { i++ } -> same_as<I>;
        { ++i } -> same_as<I&>;
    };

    template<typename I>
    concept decrementabke = requires(I i) {
        { i-- } -> same_as<I>;
        { --i } -> same_as<I&>;
    };
    
    template<typename I>
    concept input_iterator = incrementable<I> && requires(I i) {
        { *i };       // must be dereferenceable
    };
    
    template<typename O, typename T>
    concept output_iterator = incrementable<O> && requires(O o, T t) {
        { *o = t };   // must be assignable
    };
    
    template<typename I>
    concept bidirectional_iterator = input_iterator<I> && decrementabke<I>;

    template<typename I>
    concept random_access_iterator = bidirectional_iterator<I> && requires(I i, I j, int n) {
        { i += n } -> same_as<I&>;
        { j +  n } -> same_as<I>;
        { n +  j } -> same_as<I>;
        { i -= n } -> same_as<I&>;
        { j -  n } -> same_as<I>;
        {  j[n]  } -> same_as<decltype(*I{})>;
    };

}

#endif /* concepts_h */
