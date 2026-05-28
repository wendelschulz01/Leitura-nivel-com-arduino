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
            nivel_percentual,
            nivel_metros
        } = req.body;

        if(!device_id || !api_key) {
           return res.status(400).json({
                error: "device_id e api_key são obrigatórios"
            });
        }

        if(api_key !== process.env.DEVICE_API_KEY) {

            return res.status(400).json({
                error: "API key inválida"
            });
        }

        let status_leitura = "ok"
        let msg = "Leitura salva com sucesso"


        if(nivel_percentual < 0 ||
            nivel_percentual > 100 ||
            nivel_metros < 0 ||
            nivel_metros > 10 ||

            tensao < 0 ||
            tensao > 5
        ) {
            status_leitura = "alerta"
            msg = "Leitura salva, valores podem estar comprometidos"
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
                nivel_percentual,
                nivel_metros,
                ip_origem,
                status_leitura
            )
                VALUES (?,?,?,?,?,?)
            `,
            [
                device_id,
                tensao,
                nivel_percentual,
                nivel_metros,
                ipOrigem,
                status_leitura,
            ]     
        );

        res.status(201).json({
            message: msg,
            status_leitura,
            id: result.insertId
        });

    }catch(error) {
        console.error("Erro ao salvar leitura", error);

        res.status(500).json({
            error: "Erro interno no servidor"
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