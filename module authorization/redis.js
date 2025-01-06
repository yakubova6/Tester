const redis = require('redis');
const { REDIS_HOST, REDIS_PORT } = process.env;

const client = redis.createClient({
  host: REDIS_HOST,
  port: REDIS_PORT
});

client.on('error', (err) => {
  console.log('Redis error: ', err);
});

module.exports = client;
