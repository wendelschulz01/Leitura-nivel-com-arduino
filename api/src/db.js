const mysql = require("mysql2/promise");

const pool = mysql.createPool({
    host: process.env.DB_HOST || "mysql",
    port: process.env.DB_PORT || 3306,
    user: process.env.DB_USER || "sensor_user",
    password: process.env.DB_PASSWORD || "sensor_pwd",
    database: process.env.DB_NAME || "sensor_db",
    waitForConnections: true,
    connectionLimit: 10,
    queueLimit: 0
});

module.exports = pool;
