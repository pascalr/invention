#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

// Pour le mouvement: voir thirdparty/mathematics-of-motion-control-profiles-article
// Plus tard déplacer en courbe S. Pour l'instant en courbe trapézoïdale.
// Le vitesse initiale et finale (pas zéro pour un stepper motor) est
// déterminé par la variable MAX_STEP_DELAY

// La courbe n'est pas égal parce que d'un côté c'est par rapport à 10000, donc perdre 0.1 ce n'est pas
// beaucoup, mais de l'autre côté c'est 500, donc perdre 0.1 c'est beaucoup
// Je ne sais pas si c'est correct...

// Alright, using delays was not the best idea, because it is inversely proportial to speed and not linear with it.
// So I will be using percentages of speed then which are easy to translate in both units.
// 500 delay is max speed => 100%,
// 10000 delay is min speed => 5%,
// what is 50%? It's 1000, it is min_delay / percent
// What about using the step as a unit instead of time
// Also not the best because a step is not proportial to the time

#include "Motor.h"
#include "../lib/ArduinoMock.h"
#include "referencer.h"

#define MAX_STEP_DELAY 5000 // us
#define US_PER_S 1000000.0

class StepperMotor : public Motor {
  public:

    StepperMotor(Writer& writer, char theName) : Motor(writer, theName) {
      m_is_step_high = false;
      isMotorEnabled = true;
      forceRotation = false;
      m_position_steps = 0;

      m_default_max_speed = 1;
      m_acceleration = 0.2; // tour/s^2 [turn/sec^2]

    }

    // delay is inversely proportional to the speed
    // speed = distance_per_step / delay_per_step
    // but I don't care about speed, only delay

    // Source: https://www.pmdcorp.com/resources/type/articles/get/mathematics-of-motion-control-profiles-article
    // Archived here: thirdparty/mathematics-of-motion-control-profiles-article
    // For an S-curve, there are 7 phases.
    // 1. Increase of acceleration
    // 2. Max acceleration reached
    // 3. Decrease of acceleration
    // 4. Constant speed reached
    // 5. Decrease of acceleration
    // 6. Max negative acceleration reached
    // 7. Increase of acceleration back to zero
  
    int phase = 1; // what phase the motor is in

    // It is called _p and _pp to mean first and second derivative respectively
    // But careful it is not derived by time, it is derived by step
    // These are constants that should be set for each motor
    double delay_pp = 1; // second derivative of delay by step (jerk-1)
    int min_delay = 500;
    int max_delay = 10000;
    double min_delay_p = -10;
   
    // These are variables
    double delay_p = 0; // first derivative of delay by step (acceleration-1)
    double delay = 0; // (speed-1)

    double phase_3_delay = 0; // At what delay to stop phase 2 and start phase 3
    long phase_5_steps = 0; // At what distance travelled to stop phase 4 and start phase 5
    long phase_6_steps = 0; // At what distance travelled to stop phase 5 and start phase 6
    long phase_7_steps = 0; // At what distance travelled to stop phase 6 and start phase 7
    long distance_to_travel_steps = 0; // The distance to travel in steps
    long start_position_steps = 0; // At the beginning of a move, what position was it?
    bool skip_phase_2;
    long steps_to_accelerate = 0;

    int debug_delay = 0;

    long debug_time_1 = 0;
    long debug_time_2 = 0;
    long debug_time_3 = 0;
    long debug_time_4 = 0;
    long debug_time_5 = 0;
    long debug_time_6 = 0;
    long debug_time_7 = 0;

    long debug_steps_1 = 0;
    long debug_steps_2 = 0;
    long debug_steps_3 = 0;
    long debug_steps_4 = 0;
    long debug_steps_5 = 0;
    long debug_steps_6 = 0;
    long debug_steps_7 = 0;


          // To know when phase 2 should stop, let's go the other way
          // Let's say we are going at maximum speed, so delay = min_delay
          // We want to know at what speed should we start to decrease acceleration, in
          // order to not go over the maximum speed.

          // We know from phase 1 that it takes N steps to increase acceleration.
          // The same values are used for phase 3 so it takes N steps to do phase 3.
          
          // d_p = d_p0 + d_pp * s
          // d = d0 + d_p0 * s + 0.5 * d_pp * s^2
    
          // d = what we are looking for
          // d0 = min_delay
          // d_p0 = 0, the goal is to reach no acceleration at the end of phase 3
          // d_pp = delay_pp
          // s = phase1Steps
            
          //phase_3_delay = min_delay + 0.5 * delay_pp * distanceTravelledSteps * distanceTravelledSteps;
          //phase_3_delay = min_delay - 0.5 * delay_pp * distanceTravelledSteps * distanceTravelledSteps; // Why minus?
          
          
    void prepareMovement() {
      distance_to_travel_steps = (getDestination() - getPosition()) * stepsPerUnit;
      start_position_steps = m_position_steps;
      delay = max_delay;
      delay_p = 0;
      phase = 1;
    }
    
    void prepareMovementPercent() {
      distance_to_travel_steps = (getDestination() - getPosition()) * stepsPerUnit;
      percent = min_delay / max_delay;
      phaseNb = 1;
      min_percent = min_delay * 1.0 / max_delay;
    }

    // This constant must be set
    double percent_p = 0;

    int phaseNb = 1; // what phase the motor is in
   
    // These are variables
    double percent = 0;
    double min_percent = 0;

    long distance_accelerating = 0;
    double time_started_decelerating = 0;

    double debug_time_finished_accelerating_s = 0;

    double max_percent_reached = 0;

    // Using percent per time, with a trapezoidal curve
    int nextDelayPercent(long distanceTravelledSteps, unsigned long timeSinceStart) {

      double timeSinceStartS = timeSinceStart / 1000000.0;

      // Accelerating
      if (phaseNb == 1) {

        if (distanceTravelledSteps >= distance_to_travel_steps / 2.0) {
          phaseNb = 3;
          time_started_decelerating = timeSinceStartS;
          debug_time_finished_accelerating_s = timeSinceStartS;
          max_percent_reached = percent;

        } else {
          percent = percent_p * timeSinceStartS;
          if (percent >= 1.0) {
            distance_accelerating = distanceTravelledSteps;
            max_percent_reached = 1.0;
            percent = 1.0;
            phaseNb = 2;
          }
        }

      // Constant speed
      } else if (phaseNb == 2) {

        if (distanceTravelledSteps > distance_to_travel_steps - distance_accelerating) {
          phaseNb = 3;
          time_started_decelerating = timeSinceStartS;
          debug_time_finished_accelerating_s = timeSinceStartS;
        }

      // Decelerating
      } else {

        percent = max_percent_reached - percent_p * (timeSinceStartS - time_started_decelerating);

      }

      int delay = (percent <= min_percent) ? max_delay : min_delay / percent;
      //std::cout << "percent: " << percent << endl;
      debug_delay = delay;
      return delay;
    }

    // Get the next delay following an S-curve    
    int nextDelay(long distanceTravelledSteps, unsigned long timeSinceStart) {

      // Increasing acceleration
      if (phase == 1) {
        debug_time_1 = timeSinceStart;
        debug_steps_1 = distanceTravelledSteps;

        delay_p = delay_pp * distanceTravelledSteps;
        
        // Check if we reached phase 2
        if (delay_p <= min_delay_p) {

          phase_7_steps = distance_to_travel_steps - distanceTravelledSteps;
          steps_to_accelerate = distanceTravelledSteps;
          delay_p = min_delay_p;
          skip_phase_2 = false;
          phase = 2;
        }

        // If we don't decelerate now, will we go under the minimum delay?
        double delayIfDecelerate = delay + delay_p * distanceTravelledSteps - 0.5 * distanceTravelledSteps * delay_pp * distanceTravelledSteps;

        // If we have reached to quarter of the distance, skip to phase 3
        // Or if we have to start decelerating now to reach the minimum delay
        if (distanceTravelledSteps >= distance_to_travel_steps / 4.0 || delayIfDecelerate < min_delay) {
          //std::cout << "!!!!!!!!!! Skipping phase 2 !!!!!!!!!!!" << std::endl;
          skip_phase_2 = true;
          phase = 3;
        }

      // Constant acceleration
      } else if (phase == 2) {
        debug_time_2 = timeSinceStart;
        debug_steps_2 = distanceTravelledSteps;
     
        // If we don't decelerate now, will we go under the minimum delay?
        double delayIfDecelerate = delay + delay_p * steps_to_accelerate - 0.5 * steps_to_accelerate * delay_pp * steps_to_accelerate;

        if (delayIfDecelerate < min_delay) {
          phase_6_steps = distance_to_travel_steps - distanceTravelledSteps;
          phase = 3;
        }

        // Check if we reached phase 3
        //if (delay < phase_3_delay) {
        //  phase_6_steps = distance_to_travel_steps - distanceTravelledSteps;
        //  phase = 3;
        //}
        
      // Decreasing acceleration
      } else if (phase == 3) {
        debug_time_3 = timeSinceStart;
        debug_steps_3 = distanceTravelledSteps;
        
        delay_p = delay_p - delay_pp;
        
        if (delay_p >= 0) {
          //std::cout << "distance_to_travel_steps: " << distance_to_travel_steps<< endl;
          //std::cout << "DistancedTravelledSteps: " << distanceTravelledSteps << endl;
          phase_5_steps = distance_to_travel_steps - distanceTravelledSteps;
          delay_p = 0;
          phase = 4;
        }

      // Constant speed
      } else if (phase == 4) {
        debug_time_4 = timeSinceStart;
        debug_steps_4 = distanceTravelledSteps;
        
        if (distanceTravelledSteps >= phase_5_steps) phase = 5;
      
      // Decreasing acceleration
      } else if (phase == 5) {
        debug_time_5 = timeSinceStart;
        debug_steps_5 = distanceTravelledSteps;
        
        delay_p = delay_p - delay_pp;

        if (delay_p >= -min_delay_p) {
        //if (distanceTravelledSteps >= phase_6_steps) {
          delay_p = -min_delay_p;
          phase = skip_phase_2 ? 7 : 6;
        }

      // Constant acceleration
      } else if (phase == 6) {
        debug_time_6 = timeSinceStart;
        debug_steps_6 = distanceTravelledSteps;
        
        if (distanceTravelledSteps >= phase_7_steps) phase = 7;

      // Increasing acceleration
      } else if (phase == 7) {
        debug_time_7 = timeSinceStart;
        debug_time_7 = timeSinceStart;
        
        delay_p = delay_p + delay_pp;
      }
      delay = delay + delay_p;
      return delay;
    }

    void setAcceleration(double accel) {
      m_acceleration = accel;
    }

    double max_step_delay = MAX_STEP_DELAY;

    void setDefaultMaxSpeed(double s) {
      m_default_max_speed = s;
      max_step_delay = ((unsigned long)(US_PER_S / (m_default_max_speed / 5.0 * m_steps_per_turn))); // us
    }

    void setStepsPerTurn(double ratio) {
      m_steps_per_turn = ratio;
      max_step_delay = ((unsigned long)(US_PER_S / (m_default_max_speed / 5.0 * m_steps_per_turn))); // us
    }

    void setMaxSpeed(double s) {
      m_max_speed = s;
    }

    double getAcceleration() {
      return m_acceleration;
    }

    double getAccelerationInUnits() {
      return m_acceleration * m_steps_per_turn / stepsPerUnit;
    }

    double getDefaultMaxSpeed() {
      return m_default_max_speed;
    }
    
    double getMaxSpeed() {
      return m_max_speed;
    }
    

    // Linear axes units are mm. Rotary axes units are degrees.
    // Number of steps per turn of the motor * microstepping / distance per turn
    // The value is multiplied by two because we have to write LOW then HIGH for one step
    void setStepsPerUnit(double ratio) {
      stepsPerUnit = ratio;
    }

    double getStepsPerTurn() {
      return m_steps_per_turn;
    }

    double getStepsPerUnit() {
      return stepsPerUnit;
    }

    void setupPins(int enabledPin, int dirPin, int stepPin) {
      m_enabled_pin = enabledPin;
      m_dir_pin = dirPin;
      m_step_pin = stepPin;

      pinMode(m_enabled_pin, OUTPUT);
      pinMode(m_dir_pin, OUTPUT);
      pinMode(m_step_pin, OUTPUT);
    }

    void rotate(bool direction) {
      setMotorDirection(direction);
      forceRotation = true;
      setMotorEnabled(true);
    }

    double getPosition() {
      return m_position_steps / stepsPerUnit;
    }

    void stop() {
      //setMotorsEnabled(false);
      setDestination(getPosition());
      forceRotation = false;
      is_referencing = false;
    }

    void setPosition(double pos) {
      m_position_steps = pos * stepsPerUnit;
    }

    void setPositionSteps(double posSteps) {
      m_position_steps = posSteps;
    }

    long getPositionSteps() {
      return m_position_steps;
    }

    double getDestinationSteps() {
      return getDestination() * stepsPerUnit;
    }

    double getDestinationTurns() {
      return getDestination() * m_steps_per_turn;
    }

    void updateDirection() {
      setMotorDirection(getDestination() > getPosition());
    }
    
    void setMotorEnabled(bool value) {
      digitalWrite(m_enabled_pin, LOW); // FIXME: ALWAYS ENABLED
      //digitalWrite(m_enabled_pin, value ? LOW : HIGH);
      isMotorEnabled = value;
    }

  //protected:
    // Linear axes units are mm. Rotary axes units are degrees.
    double stepsPerUnit;
    double m_steps_per_turn;
    
    int m_enabled_pin;
    int m_step_pin;
    int limitSwitchPin;
    
    bool isMotorEnabled;
    bool forceRotation;

    // TODO: All the units should simply be steps... NOOOOOOOOOO All units are in turns!
    // WARNING: THIS ONLY WORKS AT THE BEGINNING, DOES NOT COMPUTE ALL THE TIME
    unsigned long timeToReachDestinationUs() {
      return m_time_to_reach_middle_us * 2;
    }

    // tours [turns]
    double distanceToReachDestination() {
      double distance = (getDestination() - getPosition()) * stepsPerUnit / m_steps_per_turn; // tr
      distance *= (distance < 0) ? -1 : 1;
      return distance;
    }

    double position_to_decelerate;
    double v_0;

    void calculateMovement() {

      double halfDistance = distanceToReachDestination() / 2;
      v_0 = US_PER_S / max_step_delay / m_steps_per_turn;

      m_max_speed_reached = sqrt(2 * m_acceleration * halfDistance + v_0*v_0); // tr/s
      
      if (m_max_speed_reached > m_max_speed) {m_max_speed_reached = m_max_speed;} // s

      double timeToAccelerate = (m_max_speed_reached - v_0) / m_acceleration; // s
      m_time_to_reach_middle_us = ((unsigned long) (timeToAccelerate * US_PER_S)); // us
      double timeConstantSpeed = 0;
      double distanceAccelerating = v_0*timeToAccelerate + 0.5 * m_acceleration * timeToAccelerate * timeToAccelerate; // tr
      bool goingForward = getDestination() > getPosition();
      position_to_decelerate += getDestination() + (distanceAccelerating * m_steps_per_turn / stepsPerUnit * (goingForward ? -1 : 1));

      if (m_max_speed_reached == m_max_speed) {

        double halfDistanceLeft = halfDistance - distanceAccelerating; // tr
        timeConstantSpeed = halfDistanceLeft / m_max_speed; // s
        m_time_to_reach_middle_us += ((unsigned long) (timeConstantSpeed * US_PER_S)); // us
      }
     
      m_time_to_start_decelerating_us = ((unsigned long)((timeConstantSpeed*2.0 + timeToAccelerate) * US_PER_S)); // us

      //std::cout << "position: " << getPosition() << std::endl;
      //std::cout << "destination: " << getDestination() << std::endl;
      //std::cout << "distanceAccelerating: " << distanceAccelerating << std::endl;
      //std::cout << "position_to_decelerate: " << position_to_decelerate << std::endl;
      //std::cout << "timeToAccelerate: " << timeToAccelerate << std::endl;
      //std::cout << "v_0: " << v_0 << std::endl;
      //std::cout << "acceleration: " << m_acceleration << std::endl;
    }

    int setDestination(double dest) {
      if (is_referenced == false) {return ERROR_AXIS_NOT_REFERENCED;}

      int status = Motor::setDestination(dest);
      if (status < 0) {return status;}

      //calculateMovement();
      //prepareMovement();
      prepareMovementPercent();
      return 0;
    }

    double getCurrentSpeed() {
      return m_speed;
    }

    //virtual unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {

    //  double distanceDecelerating = 0.5 * m_speed * m_speed / m_acceleration; // tr
    //  double position = getPosition();
    //  double distanceToGo = abs(getDestination() - position);
    //  double distanceToCollision = isForward ? getMaxPosition() - position : position - getMinPosition();

    //  // decelerate
    //  if (distanceDecelerating > distanceToGo || distanceDecelerating > distanceToCollision) {
    //   
    //    cout << "Decelerating!" << endl;
    //    m_speed -= m_acceleration * (m_next_step_time / 1000000.0); // tr/s

    //  // accelerate
    //  } else if (m_speed < getMaxSpeed()) {

    //    cout << "Accelerating!" << endl;
    //    m_speed += m_acceleration * (m_next_step_time / 1000000.0); // tr/s
    //  }
    //  
    //  unsigned long stepDelay = ((unsigned long)(1000000.0 / (m_speed * m_steps_per_turn))); // us

    //  return stepDelay > max_step_delay ? max_step_delay : stepDelay;
    //}

    //unsigned int microsteps = 16;
    
    unsigned long calculateNextStepDelay(unsigned long timeSinceStart) {

      if (forceRotation) {return max_step_delay;}

      // Decelerate
      if (timeSinceStart > m_time_to_start_decelerating_us) {
      //if (getPosition() > position_to_decelerate) {

        //std::cout << "Decelerating.\n";
        double t_s = (timeSinceStart - m_time_to_start_decelerating_us) / US_PER_S; // s
        m_speed = m_max_speed_reached - (m_acceleration * t_s); // tr/s
      
      // Accelerate or go top speed
      } else {

        m_speed = v_0 + m_acceleration * (timeSinceStart / US_PER_S); // tr/s
        if (m_speed > m_max_speed) {m_speed = m_max_speed;} // tr/s
      }

      unsigned long stepDelay = ((unsigned long)(US_PER_S / (m_speed * m_steps_per_turn))); // us

      return stepDelay > max_step_delay ? max_step_delay : stepDelay;
    }

    unsigned long next_step_delay = 0;
    unsigned long lost_time = 0;

    void turnOneStep(unsigned long timeSinceStart) {
      digitalWrite(m_step_pin, m_is_step_high ? LOW : HIGH);
      m_is_step_high = !m_is_step_high;
      m_position_steps = m_position_steps + (isForward ? 1 : -1);

      lost_time = timeSinceStart - next_step_time;
      next_step_delay = nextDelayPercent(m_position_steps - start_position_steps, timeSinceStart);
      //next_step_delay = nextDelay(m_position_steps - start_position_steps);
      //next_step_delay = calculateNextStepDelay(timeSinceStart); 
      next_step_time = timeSinceStart + next_step_delay - lost_time;
    }

    bool m_is_step_high;

    long m_position_steps;

    double m_max_speed_reached;
    unsigned long next_step_time; // us
    unsigned long m_start_time; // us
    unsigned long m_time_to_reach_middle_us; // us
    unsigned long m_time_to_start_decelerating_us; // us
   
    double m_acceleration; // tour/s^2 [turn/sec^2]
    double m_max_speed; // tour/s [turn/sec]
    double m_default_max_speed; // tour/s [turn/sec]
    double m_speed;

    LimitSwitchReferencer referencer;
    Referencer& getReferencer() {
      return referencer;
    }

    // TODO:
    // bool m_jog_forward; No more forceRotation.
    // bool m_jog_reverse; No more isForward.
    
    // Returns true if the axis is still working.
    virtual bool handleAxis(unsigned long currentTime) {
      //unsigned int delay = getDelay();

      if (handleReferencing(currentTime)) {return true;}
      
      if (forceRotation || !isDestinationReached()) {
        unsigned long timeSinceStart = timeDifference(m_start_time, currentTime); // us
        if (timeSinceStart >= next_step_time) {
          turnOneStep(timeSinceStart);
        }
        return true;
      } else {
        m_speed = 0;
      }
      return false;
    }

  protected:

    void doStartReferencing() {
    }

    // Resets some stuff.
    virtual void prepare(unsigned long time) {
      m_start_time = time;
      next_step_time = 0;
      m_speed = 0;
      m_max_speed = m_default_max_speed;
    }
   
    // Pretty sure this does not work... 
    virtual void run(unsigned long currentTime, double speedRPM) {
      next_step_time = ((unsigned long)(US_PER_S / (speedRPM / 60.0 * m_steps_per_turn))); // us
      unsigned long timeSinceStart = timeDifference(m_start_time, currentTime); // us
      if (currentTime >= next_step_time) {
        turnOneStep(timeSinceStart);
      }
    }

};

class MotorT : public StepperMotor {
  public:

    MotorT(Writer& writer, char theName) : StepperMotor(writer, theName) {
    }

    void doStartReferencing() {
      referenceReached();
    }

    void referenceReached() {
      Motor::referenceReached();

      //int total = 100;
      //double sum;
      //for (int i = 0; i < total; i++) {
      //  sum += analogRead(PIN_THETA_POT);
      //}
      //double mean = sum/total;
      //double degreesPerAnalog = 180.0 / (POT_180_VAL - POT_0_VAL);
      //double pos = degreesPerAnalog * mean;
      double pos = 90.0;
      
      setPosition(pos);
      setDestination(pos);
    }

};

#endif
