template <class T> SquareNuduplOpt<T>::SquareNuduplOpt(QuadraticOrder<T> &inQO) {
  Delta = inQO.get_discriminant();
  NC_BOUND = FloorToZZ(sqrt(sqrt(abs(to_RR(Delta)))));
}
