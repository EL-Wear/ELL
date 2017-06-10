////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FullyConnectedLayer.tcc (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FullyConnectedLayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    template <typename ElementType>
    FullyConnectedLayer<ElementType>::FullyConnectedLayer(const LayerParameters& layerParameters, MatrixReferenceType& weights) :
        Layer<ElementType>(layerParameters),
        _weights(weights.NumRows(), weights.NumColumns()),
        _shapedInput(layerParameters.input.NumElements()),
        _outputVector(GetOutputMinusPadding().NumElements())
    {
        _weights = weights;
        if (_weights.NumRows() != (GetOutputMinusPadding().NumElements()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "weights dimension for a fully connected layer should be the same as number of output nodes times inputs per node");
        }
    }

    template <typename ElementType>
    FullyConnectedLayer<ElementType>::FullyConnectedLayer(const LayerParameters& layerParameters, ConstTensorReferenceType& weights) :
        Layer<ElementType>(layerParameters),
        _weights(GetOutputMinusPadding().NumElements(), layerParameters.input.NumElements()),
        _shapedInput(layerParameters.input.NumElements()),
        _outputVector(GetOutputMinusPadding().NumElements())
    {
        // Reshape the weights into the _weights matrix
        // Each row is represents an output neuron, each column corresponds to the weight for that input
        const size_t rowIncrement = layerParameters.input.NumColumns() * layerParameters.input.NumChannels();
        const size_t columnIncrement = layerParameters.input.NumChannels();
        for (size_t outRow = 0; outRow < _weights.NumRows(); outRow++)
        {
            for (size_t i = 0; i < layerParameters.input.NumRows(); i++)
            {
                for (size_t j = 0; j < layerParameters.input.NumColumns(); j++)
                {
                    for (size_t k = 0; k < layerParameters.input.NumChannels(); k++)
                    {
                        size_t column = (i * rowIncrement) + (j * columnIncrement) + k;
                        _weights(outRow, column) = weights(outRow * layerParameters.input.NumRows() + i, j, k);
                    }
                }
            }
        }
    }    

    template <typename ElementType>
    void FullyConnectedLayer<ElementType>::Compute()
    {
        auto output = GetOutputMinusPadding();
        auto& input = _layerParameters.input;

        // Reshape the input into a vector
        size_t columnIndex = 0;
        for (size_t i = 0; i < input.NumRows(); i++)
        {
            for (size_t j = 0; j < input.NumColumns(); j++)
            {
                for (size_t k = 0; k < input.NumChannels(); k++)
                {
                    _shapedInput[columnIndex++] = input(i, j, k);
                }
            }
        }

        math::Operations::Multiply((ElementType)1.0f, _weights, _shapedInput, (ElementType)0.0f, _outputVector);

        // Reshape the output
        columnIndex = 0;
        for (size_t i = 0; i < output.NumRows(); i++)
        {
            for (size_t j = 0; j < output.NumColumns(); j++)
            {
                for (size_t k = 0; k < output.NumChannels(); k++)
                {
                    output(i, j, k) = _outputVector[columnIndex++];
                }
            }
        }
    }

    template <typename ElementType>
    void FullyConnectedLayer<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        // TODO:
    }

    template <typename ElementType>
    void FullyConnectedLayer<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        // TODO:
    }

}
}
}

