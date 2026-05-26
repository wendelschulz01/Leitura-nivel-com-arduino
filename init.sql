CREATE TABLE IF NOT EXISTS leituras_sensor (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(100) NOT NULL,
    tensao DECIMAL (10, 4),
    corrente_ma DECIMAL (10, 4),
    nivel_percentual (10,2),
    ip_origem VARCHAR(100),
    criado_em TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);