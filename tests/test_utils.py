import sys
import os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from app.utils import carregar_dados
import pandas as pd

def test_carregar_dados():
    df = carregar_dados()
    assert isinstance(df, pd.DataFrame)
    assert not df.empty
    assert 'timestamp' in df.columns
    assert 'temperatura' in df.columns
    assert 'umidade' in df.columns