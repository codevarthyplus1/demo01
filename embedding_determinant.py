#!/usr/bin/env python3
"""
Calculate the determinant of the LLaMA 3 embedding matrix multiplied by its transpose.

This script:
1. Loads the LLaMA 3 model
2. Extracts the embedding matrix E (vocab_size × embedding_dim)
3. Computes E^T @ E (embedding_dim × embedding_dim)
4. Calculates the determinant of the result
"""

import numpy as np
import torch
from transformers import AutoModel, AutoTokenizer
import argparse


def load_llama3_embeddings(model_name="meta-llama/Meta-Llama-3-8B"):
    """
    Load the embedding matrix from LLaMA 3 model.

    Args:
        model_name: HuggingFace model identifier

    Returns:
        numpy array of shape (vocab_size, embedding_dim)
    """
    print(f"Loading model: {model_name}")

    try:
        # Load the model
        model = AutoModel.from_pretrained(model_name, torch_dtype=torch.float32)

        # Extract embedding layer
        # LLaMA models use model.embed_tokens for the embedding layer
        embedding_layer = model.embed_tokens
        embedding_matrix = embedding_layer.weight.detach().cpu().numpy()

        print(f"Embedding matrix shape: {embedding_matrix.shape}")
        print(f"  Vocabulary size: {embedding_matrix.shape[0]}")
        print(f"  Embedding dimension: {embedding_matrix.shape[1]}")

        return embedding_matrix

    except Exception as e:
        print(f"Error loading model: {e}")
        print("\nNote: You may need to authenticate with HuggingFace and request access to LLaMA 3 models.")
        print("Alternatives:")
        print("  1. Use 'huggingface-cli login' to authenticate")
        print("  2. Request access at https://huggingface.co/meta-llama/Meta-Llama-3-8B")
        raise


def compute_gram_determinant(embedding_matrix):
    """
    Compute the determinant of E^T @ E where E is the embedding matrix.

    This creates a Gram matrix of dimension (embedding_dim × embedding_dim).

    Args:
        embedding_matrix: numpy array of shape (vocab_size, embedding_dim)

    Returns:
        determinant value
    """
    print("\nComputing Gram matrix (E^T @ E)...")

    # E is (vocab_size, embedding_dim)
    # E^T @ E is (embedding_dim, embedding_dim)
    gram_matrix = embedding_matrix.T @ embedding_matrix

    print(f"Gram matrix shape: {gram_matrix.shape}")

    print("\nCalculating determinant...")
    det = np.linalg.det(gram_matrix)

    print(f"Determinant of E^T @ E: {det}")
    print(f"Determinant (scientific notation): {det:.6e}")

    # Additional information
    print(f"\nLog determinant: {np.linalg.slogdet(gram_matrix)[1]}")

    return det


def main():
    parser = argparse.ArgumentParser(
        description="Calculate determinant of LLaMA 3 embedding Gram matrix"
    )
    parser.add_argument(
        "--model",
        type=str,
        default="meta-llama/Meta-Llama-3-8B",
        help="HuggingFace model identifier (default: meta-llama/Meta-Llama-3-8B)"
    )

    args = parser.parse_args()

    # Load embeddings
    embedding_matrix = load_llama3_embeddings(args.model)

    # Compute determinant
    determinant = compute_gram_determinant(embedding_matrix)

    # Save results
    results = {
        'model': args.model,
        'embedding_shape': embedding_matrix.shape,
        'gram_matrix_shape': (embedding_matrix.shape[1], embedding_matrix.shape[1]),
        'determinant': float(determinant),
        'log_determinant': float(np.linalg.slogdet(embedding_matrix.T @ embedding_matrix)[1])
    }

    print("\n" + "="*60)
    print("RESULTS")
    print("="*60)
    for key, value in results.items():
        print(f"{key}: {value}")

    # Save to file
    import json
    with open('embedding_determinant_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    print("\nResults saved to: embedding_determinant_results.json")


if __name__ == "__main__":
    main()
