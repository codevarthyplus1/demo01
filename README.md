# LLaMA 3 Embedding Matrix Determinant Calculator

This project calculates the determinant of the LLaMA 3 embedding matrix multiplied by its transpose.

## Mathematical Operation

Given the embedding matrix `E` with shape `(vocab_size, embedding_dim)`:

1. Compute the Gram matrix: `G = E^T @ E` (shape: `embedding_dim × embedding_dim`)
2. Calculate: `det(G)`

The Gram matrix represents the inner products between all pairs of embedding dimensions.

## Setup

### Prerequisites

- Python 3.8+
- HuggingFace account with access to LLaMA 3 models

### Installation

```bash
pip install -r requirements.txt
```

### Authentication

You need to authenticate with HuggingFace and request access to LLaMA 3:

1. Request access at: https://huggingface.co/meta-llama/Meta-Llama-3-8B
2. Login via CLI:
   ```bash
   huggingface-cli login
   ```

## Usage

Run the script with default settings (Meta-Llama-3-8B):

```bash
python embedding_determinant.py
```

Or specify a different LLaMA 3 model:

```bash
python embedding_determinant.py --model meta-llama/Meta-Llama-3-70B
```

## Output

The script will:
- Load the LLaMA 3 model
- Extract the embedding matrix
- Compute the Gram matrix (E^T @ E)
- Calculate and display the determinant
- Save results to `embedding_determinant_results.json`

### Example Output

```
Loading model: meta-llama/Meta-Llama-3-8B
Embedding matrix shape: (128256, 4096)
  Vocabulary size: 128256
  Embedding dimension: 4096

Computing Gram matrix (E^T @ E)...
Gram matrix shape: (4096, 4096)

Calculating determinant...
Determinant of E^T @ E: <value>
```

## Notes

- The determinant is computed for E^T @ E (not E @ E^T) because:
  - E^T @ E has shape (embedding_dim × embedding_dim) ≈ (4096 × 4096)
  - E @ E^T has shape (vocab_size × vocab_size) ≈ (128256 × 128256)
  - The smaller matrix is more computationally feasible
- Very large or very small determinant values are common and expressed in scientific notation
- The log determinant is also provided for numerical stability
