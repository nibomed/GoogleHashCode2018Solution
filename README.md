#GoogleHashCode2018Solution

Self-driving rides

More than 48 million score.

Main idea - find best ride for each car when car becomes empty.
To find best we calculate some rate. Biggest rate - better ride.

                                  how_much_score_I_will_get_if_car_took_this_ride_right_now
    basicRate = f(car, ride) =  -------------------------------------------------------------
                                     how_much_time_car_will_lose_to_finish_this_ride


    progressiveRate = r(car, current_ride) := f(car, current_ride) + Max(f(CAR, ride0), f(CAR, ride1), ... f(CAR, rideN))
       where CAR - state of car after finishing current_ride
             ride0, ride1, ... rideN - orders that have sense after finishing current_ride

basicRate bad where we have long rides. Like in metropolis test.