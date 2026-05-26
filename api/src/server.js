require("dotenv").config();

const express = require("express");
const cors = require("cors");
const pool = require("./db");

const app = express();

app.use(cors());
app.use(express.json());

const PORT = process.env.API_PORT || 4000;

app.get("/health", (req, res) => {
    res.json({
        status: "ok",
        service: "sensor-api"
    });
});

app.post("/api/readings", async (req,res) => {
    try {
        const {
            device_id,
            api_key,
            tensao,
            corrente_ma,
            nivel_percentual
        } = req.body;

        if(!device_id || !api_key) {
            res.status(400).json({
                error: "device_id e api_key são obrigatórios"
            });
        }

        if(api_key !== process.env.DEVICE_API_KEY) {
            return res.status(400).json({
                error: "API key inválida"
            });
        }

        if(nivel_percentual < 0 ||
            nivel_percentual > 100 ||
            corrente_ma < 3 ||
            corrente_ma > 22
        ) {
            return res.status(400).json({
                error: "valores fora da faixa esperada"
            });
        }

        const ipOrigem  = 
        req.headers["x-forwarded-for"] ||
        req.socket.remoteAddress;

        const [result] = await pool.execute (
            `
            INSERT  INTO leituras_sensor
            (
                device_id,
                tensao,
                corrente_ma,
                nivel_percentual,
                ip_origem
            )
                VALUES (?,?,?,?,?)
            `,
            [
                device_id,
                tensao,
                corrente_ma,
                nivel_percentual,
                ipOrigem,
            ]     
        );

        res.status(201).json({
            message: "Leitura salva com sucesso",
            id: result.insertId
        });

    }catch(error) {
        console.error("Erro ao salvar leitura", error);

        res.status(500).json({
            error: "Erro interno no servidor    "
        });
    }
});

app.get("/api/readings/:id", async (req, res) =>{

    const id = req.params.id;


    try {
        const [rows] = await pool.execute(
            `
            SELECT * FROM leituras_sensor
            WHERE id = ?
            `, 
            [id]
        );

        res.json(rows);

    } catch(error) {
        console.log("Erro ao buscar leituras", error);

        res.status(500).json({
            error: "Erro ao buscar leituras"
        });
    }
});

app.get("/api/readings/", async (req, res) =>{

    try {
        const [rows] = await pool.execute(
            `
            SELECT * FROM leituras_sensor
            ORDER BY criado_em DESC
            LIMIT 100
            `
        );

        res.json(rows);

    } catch(error) {
        console.log("Erro ao buscar leituras", error);

        res.status(500).json({
            error: "Erro ao buscar leituras"
        });
    }
});

app.listen (PORT, () => {
    console.log(`API rodando na porta ${PORT}`);
});