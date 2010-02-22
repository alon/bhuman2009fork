/** 
 * @file Kalman.h
 * Template classes for very simple kalman filtering.
 * @author Colin Graf
 */
 
/**
* @class Kalman
* Implements a very simple kalman filter.
*/
template <class V = double> class Kalman
{
public:
  V value; /**< The estimated state. */
  V variance; /**< The current variance. */

  /** 
  * Default constructor.
  */
  Kalman() : value(V()), variance(V()) {}

  /** 
  * Constructor.
  * @param value The initial state.
  * @param variance The initial variance.
  */
  Kalman(const V& value, const V& variance) : value(value), variance(variance) {}
  
  void init(const V& value, const V& variance)
  {
    this->value = value;
    this->variance = variance;
  }

  /**
  * Performs a prediction step.
  * @param u A dynamic measurement.
  * @param uVariance The variance of this measurement.
  */
  void predict(const V& u, const V& uVariance)
  {
    value += u;
    variance += uVariance;
  }
  
  /**
  * Performs an update step.
  * @param z An absolute measurement.
  * @param zVariance The variance of this measurement.
  */
  void update(const V& z, const V& zVariance)
  {
    const V& k(variance / (variance + zVariance));
    value += k * (z - value);
    variance -= k * variance;
  }
};
