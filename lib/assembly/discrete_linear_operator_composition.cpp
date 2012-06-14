// Copyright (C) 2011-2012 by the BEM++ Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "config_trilinos.hpp"

#include "discrete_linear_operator_composition.hpp"
#include "../fiber/explicit_instantiation.hpp"

namespace Bempp
{

template <typename ValueType>
DiscreteLinearOperatorComposition<ValueType>::
DiscreteLinearOperatorComposition(shared_ptr<const Base> &outer,
                                  shared_ptr<const Base> &inner) :
    m_outer(outer), m_inner(inner)
{
    if (!m_outer || !m_inner)
        throw std::invalid_argument(
                "DiscreteLinearOperatorComposition::DiscreteLinearOperatorComposition(): "
                "arguments must not be NULL");
    if (m_outer->columnCount() != m_inner->rowCount())
        throw std::invalid_argument(
                "DiscreteLinearOperatorComposition::DiscreteLinearOperatorComposition(): "
                "term dimensions do not match");
    // TODO: perhaps test for compatibility of Thyra spaces
}

template <typename ValueType>
arma::Mat<ValueType>
DiscreteLinearOperatorComposition<ValueType>::asMatrix() const
{
    return m_outer->asMatrix() * m_inner->asMatrix();
}

template <typename ValueType>
unsigned int
DiscreteLinearOperatorComposition<ValueType>::rowCount() const
{
    return m_outer->rowCount();
}

template <typename ValueType>
unsigned int
DiscreteLinearOperatorComposition<ValueType>::columnCount() const
{
    return m_inner->columnCount();
}

template <typename ValueType>
void DiscreteLinearOperatorComposition<ValueType>::addBlock(
        const std::vector<int>& rows,
        const std::vector<int>& cols,
        const ValueType alpha,
        arma::Mat<ValueType>& block) const
{
    throw std::runtime_error(
                "DiscreteLinearOperatorComposition::DiscreteLinearOperatorComposition(): "
                "addBlock: not implemented yet");
}

#ifdef WITH_TRILINOS
template <typename ValueType>
Teuchos::RCP<const Thyra::VectorSpaceBase<ValueType> >
DiscreteLinearOperatorComposition<ValueType>::domain() const
{
    return m_inner->domain();
}

template <typename ValueType>
Teuchos::RCP<const Thyra::VectorSpaceBase<ValueType> >
DiscreteLinearOperatorComposition<ValueType>::range() const
{
    return m_outer->range();
}

template <typename ValueType>
bool DiscreteLinearOperatorComposition<ValueType>::opSupportedImpl(
        Thyra::EOpTransp M_trans) const
{
    return (m_outer->opSupported(M_trans) &&
            m_inner->opSupported(M_trans));
}
#endif // WITH_TRILINOS

template <typename ValueType>
void DiscreteLinearOperatorComposition<ValueType>::
applyBuiltInImpl(const TranspositionMode trans,
                 const arma::Col<ValueType>& x_in,
                 arma::Col<ValueType>& y_inout,
                 const ValueType alpha,
                 const ValueType beta) const
{
    if (beta == static_cast<ValueType>(0.))
        y_inout.fill(0.);
    else
        y_inout *= beta;

    if (trans == TRANSPOSE || trans == CONJUGATE_TRANSPOSE) {
        m_outer->apply(trans, x_in, y_inout, alpha, 1.);
        m_inner->apply(trans, x_in, y_inout, 1., 1.);
    } else {
        m_inner->apply(trans, x_in, y_inout, alpha, 1.);
        m_outer->apply(trans, x_in, y_inout, 1., 1.);
    }
}

FIBER_INSTANTIATE_CLASS_TEMPLATED_ON_RESULT(DiscreteLinearOperatorComposition);

} // namespace Bempp
