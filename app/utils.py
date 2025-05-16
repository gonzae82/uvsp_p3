import pandas as pd
import requests
import pytz
from datetime import datetime
import sys
import os

# Adiciona a raiz do projeto ao path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from core.config import SUPABASE_URL, HEADERS

def carregar_dados():
    url = f"{SUPABASE_URL}/rest/v1/sensores?select=*"
    res = requests.get(url, headers=HEADERS)
    res.raise_for_status()  # Importante para validar se houve erro HTTP

    df = pd.DataFrame(res.json())

    # Converte para datetime com fuso horário UTC
    df['timestamp'] = pd.to_datetime(df['timestamp'], utc=True)

    # Converte para horário de Brasília (GMT-3 ou -2 dependendo do horário de verão)
    fuso_brasilia = pytz.timezone("America/Sao_Paulo")
    df['timestamp'] = df['timestamp'].dt.tz_convert(fuso_brasilia)
    
    return df