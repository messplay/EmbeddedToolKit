#ifndef ETK_FUZZY_H
#define ETK_FUZZY_H

#include <etk/list.h>

namespace etk
{

/**
 * \class Fuzzy
 *
 * \brief Fuzzy logic class. Fuzzy logic can be used for function approximation, control applications, signal process, AI and so on.
 * Here is a tutorial on using Fuzzy logic with ETK
 * http://www.camelsoftware.com/blog/2015/12/12/fuzzy-logic-control-part-1/
 *
 * @tparam N The maximum number of fuzzy sets to use. 
 */
 
 
template <uint16_t N> class Fuzzy
{
public:

/**
 * \class Set
 *
 * \brief A fuzzy logic set.
 *
 */
 
    class Set
    {
    friend class Fuzzy<N>;
    
    public:
    	
		/**
		 * \brief set_points modifies the range and mid point of a fuzzy logic set. This controls the shape of the triangle. 
		 *
		 * @arg p1 minimum point
		 * @arg p2 middle/peak of triangle
		 * @arg p3 maximum point
		 */
        void set_points(float p1, float p2, float p3)
        {
            min = p1;
            mid = p2;
            max = p3;
        }

		/**
		 * \brief get_dom returns the degree of membership of a crisp input. If the crisp input is less than the minimum point or greather than the maximum point, the DOM is zero. 
		 * Otherwise, the DOM represents how close the crisp is to the mid point. 
		 *
		 * @arg crisp_in The crisp input
		 * @return The degree of membership. This will be a number between 0.0 and 1.0
		 */
        float get_dom(float crisp_in)
        {
            if(pos == MID)
            {
                if(crisp_in < min)
                    return 0;
                if(crisp_in > max)
                    return 0;
            }

            if(pos == START)
            {
                if(crisp_in < mid)
                    return 1;
            }
            if(pos == END)
            {
                if(crisp_in >= mid)
                    return 1;
            }

            if(crisp_in > mid)
            {
                float range = max-mid;
                crisp_in -= mid;
                crisp_in /= range;
            }
            else
            {
                float range = mid-min;
                crisp_in -= mid;
                crisp_in /= range;
            }
            if(crisp_in < 0)
                crisp_in *= -1;

            if(crisp_in > 1)
                crisp_in = 1;

            return (1-crisp_in);
        }
        
        /**
         * \brief Sets the value of this set.
         */
        void set_value(float v)
        {
        	val = v;
        }

		/**
		 * \brief Returns the degree of membership of a crisp input, multiplied by the value of the set.
		 */
        float get_result(float crisp_in)
        {
            return get_dom(crisp_in)*val;
        }
        
        /**
         * \brief Returns the minimum point.
         */
        float get_min()
        {
        	return min;
        }
        
        /**
         * \brief Returns the middle point / triangle peak.
         */
        float get_mid()
        {
        	return mid;
        }
        
        /**
         * \brief Returns the maximum point.
         */
        float get_max()
        {
        	return max;
        }

	protected:
		enum FUZZY_POINT { START,MID,END };
		
		void set_position(FUZZY_POINT p)
        {
        	pos = p;
        }
        
	private:
        float min, mid, max;
        FUZZY_POINT pos;
        float val;
    };


	/**
	 * \brief Adds a set. You can add up to N sets.
	 */
    void add_set(Set& f)
    {
        sets.append(f);
        sets[0].set_position(Set::START);
        int32_t end = sets.size()-1;
        for (auto i : range(end))
            sets[i].set_position(Set::MID);
        sets[sets.size()-1].set_position(Set::END);
    }

	/**
	 * \brief Returns the number of sets.
	 */
    uint32_t get_n_sets()
    {
        return sets.size();
    }

	/**
	 * \brief Removes all sets.
	 */
    void clear_sets()
    {
        sets.clear();
    }

	/**
	 * \brief Returns a crisp output for a given crisp input.
	 * @arg crisp_in A crisp input.
	 * @return A crisp output.
	 */
    float crisp_out(float crisp_in)
    {
        auto out = 0.0f;
        for(auto set : sets)
            out += set.get_result(crisp_in);
        return out;
    }
    
    /**
     * \brief Returns a fuzzy logic class that is inverted. 
     * Let's say you're using Fuzzy to control speed using throttle. You know for throttle x you get speed y. 
     * You can plot the throttle response using Fuzzy, but all that's going to tell is you what speed to expect for a selected throttle setting.
     * By inverting the Fuzzy sets, you can work backwards and determine what throttle setting is required for a selected speed. 
     * @return A Fuzzy class that plots an inverse function. 
     */
    auto inverse()
    {
    	auto ret = *this;
    	for(auto& set : ret.sets)
    	{
    		float set_min = set.get_min();
    		float set_mid = set.get_mid();
    		float set_max = set.get_max();
    		set.set_points(crisp_out(set_min), crisp_out(set_mid), crisp_out(set_max));
    		set.set_value(set_mid);
    	}
    	return ret;
    }

private:

    etk::List<Set, N+1> sets;
};

};


#endif

