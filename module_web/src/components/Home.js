import React from 'react';
import './styles/Home.css';
import GitHubAuth from './buttons/GitHubAuth';
import YandexAuth from './buttons/YandexAuth';
import CodeAuth from './buttons/CodeAuth';

const Home = ({ userStatus }) => {
    return (
        <div className="container">
            <h1>Добро пожаловать!</h1>
            {userStatus === 'unknown' && (
                <>
                    <p>Вы не авторизованы. Выберите способ входа:</p>
                    <GitHubAuth />
                    <YandexAuth />
                    <CodeAuth />
                </>
            )}
            {userStatus === 'anonymous' && (
                <>
                    <p>Вы вошли как анонимный пользователь. Пройдите авторизацию:</p>
                    <GitHubAuth />
                    <YandexAuth />
                </>
            )}
            {userStatus === 'authorized' && (
                <>
                    <p>Вы авторизованы!</p>
                    <a href="/dashboard">Перейти в личный кабинет</a>
                    <br />
                    <a href="/logout">Выйти</a>
                </>
            )}
        </div>
    );
};

export default Home;