#ifndef QUAD_RELATION_GENERATOR_H
#define QUAD_RELATION_GENERATOR_H

#include "ANTL/IndexCalculus/RelationGenerator/RelationGenerator.hpp"
#include "ANTL/IndexCalculus/Relation/QuadRelation.hpp"
#include "ANTL/IndexCalculus/FactorBase/QuadFactorBase.hpp"
#include "ANTL/Constants.hpp"

template < class T >
class QuadraticOrder; // an order that inherits from IOrder

namespace ANTL
{

class QuadRelationGenerator : public RelationGenerator {
public:
  QuadRelationGenerator(IOrder const &order, std::map<std::string, std::string> const &params, QuadFactorBase const &fb) :
  FB(fb), RelationGenerator(order, params) {};

  QuadRelationGenerator & operator = (const QuadRelationGenerator &fb);

  virtual bool get_relation(Relation &rel, long &num_tests) {};
private:
  // factor base associated to this relation generator
  QuadFactorBase const &FB;
};

} // ANTL

#include "src/IndexCalculus/RelationGenerator/QuadRelationGenerator_impl.hpp"

#endif // guard
