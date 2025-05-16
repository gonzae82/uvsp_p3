import streamlit as st
import pandas as pd
import plotly.express as px
import requests
from datetime import timedelta
import pytz
import sys
import os

# Adiciona a raiz do projeto ao path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

# Importa da config
from core.config import SUPABASE_URL, SUPABASE_KEY

# Importa função do utilitário
from app.utils import carregar_dados

headers = {
    "apikey": SUPABASE_KEY,
    "Authorization": f"Bearer {SUPABASE_KEY}"
}

# Configurações do Streamlit
st.set_page_config(page_title="Monitoramento de Sensores", page_icon=":thermometer:", layout="centered")
st.title(":thermometer: Monitoramento de Sensores  ")

st.subheader("Projeto Integrador em Computação III - DRP14 - Grupo 10", divider="orange")

# Botão de atualização
if st.button("🔄 Atualizar Dados"):
    dados = carregar_dados()
  
else:
    dados = carregar_dados()

st.markdown("<hr/>", unsafe_allow_html=True)

# Carrega os dados
dados = carregar_dados()

# Ordena pelo timestamp
dados = dados.sort_values(by="timestamp")

# Último dado
ultimo = dados.iloc[-1]

#Define o limite de 1 hora antes
uma_hora_atras = ultimo['timestamp'] - timedelta(hours=1)

# Filtra os dados até 1 hora antes
dados_passado = dados[dados['timestamp'] <= uma_hora_atras]

if not dados_passado.empty:
    # Pega o mais próximo (mais recente antes de uma hora)
    referencia = dados_passado.iloc[-1]
    delta_temp = ultimo['temperatura'] - referencia['temperatura']
    delta_umid = ultimo['umidade'] - referencia['umidade']
else:
    delta_temp = 0
    delta_umid = 0

col1, col2, col3, col4 = st.columns(4)
col1.metric("Temperatura", f"{ultimo['temperatura']} °C", f"{delta_temp:+.1f} °C/h")
col4.metric("Umidade", f"{ultimo['umidade']} %", f"{delta_umid:+.1f} %/h")

st.markdown("<hr/>", unsafe_allow_html=True)

# Configura data e hora para padrão BR
dados['data'] = dados['timestamp'].dt.strftime('%d/%m/%Y')
dados['hora'] = dados['timestamp'].dt.strftime('%H:%M:%S')

st.dataframe(
            dados,
            hide_index=True,
            use_container_width=True,
            column_order=["data", "hora", "temperatura", "umidade"],
            column_config={
                "data": st.column_config.TextColumn("📅 Data"),
                "hora": st.column_config.TextColumn("⏰ Hora"),
                "temperatura": st.column_config.NumberColumn("🌡️ Temperatura (°C)"),
                "umidade": st.column_config.NumberColumn("💧 Umidade (%)"),      
            }               
)


#fig = px.line(dados, x="timestamp", y=["temperatura", "umidade"], markers=True)
#st.plotly_chart(fig, use_container_width=True)



