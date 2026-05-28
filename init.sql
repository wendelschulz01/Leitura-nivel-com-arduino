CREATE TABLE IF NOT EXISTS leituras_sensor (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(100) NOT NULL,
    tensao DECIMAL (10, 4),
    nivel_percentual DECIMAL(10,2),
    nivel_metros DECIMAL (10, 2),
    ip_origem VARCHAR(100),
    status_leitura VARCHAR(50) DEFAULT 'ok',
    criado_em TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);