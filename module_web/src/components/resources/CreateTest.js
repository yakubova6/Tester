import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import '../styles/CreateTest.css';

const CreateTest = ({ permissions }) => {
    const navigate = useNavigate();
    const [testName, setTestName] = useState('');
    const [testId, setTestId] = useState(null);
    const [questions, setQuestions] = useState([{ question: '', answers: [''], correctAnswer: 0 }]);

    // Проверка наличия permissions
    if (!permissions || !Array.isArray(permissions)) {
        return <div>Ошибка: отсутствуют права доступа.</div>;
    }

    // Создание теста
    const handleCreateTest = async () => {
        if (!permissions.includes('test:add')) {
            alert('У вас нет доступа для создания теста.');
            return;
        }
        try {
            const response = await axios.post('/api/tests', { name: testName }, { withCredentials: true });
            setTestId(response.data.id); // Сохраняем ID созданного теста
            alert('Тест успешно создан! Теперь добавьте вопросы.');
        } catch (error) {
            console.error('Ошибка при создании теста:', error);
            alert('Не удалось создать тест.');
        }
    };

    // Добавление вопроса
    const handleAddQuestion = () => {
        setQuestions(prev => [...prev, { question: '', answers: [''], correctAnswer: 0 }]);
    };

    // Добавление ответа
    const handleAddAnswer = (index) => {
        const newQuestions = [...questions];
        newQuestions[index].answers.push(''); // Добавляем новый пустой ответ
        setQuestions(newQuestions);
    };

    // Сохранение вопросов
    const handleSaveQuestions = async () => {
        if (!testId) {
            alert('Сначала создайте тест.');
            return;
        }
        try {
            await axios.put(`/api/tests/${testId}/questions`, { questions }, { withCredentials: true });
            navigate('/tests'); // Перенаправляем на страницу тестов после добавления вопросов
        } catch (error) {
            console.error('Ошибка при добавлении вопросов:', error);
            alert('Не удалось добавить вопросы.');
        }
    };

    return (
        <div className="create-test-container">
            <h2>Создать тест</h2>
            {!testId ? (
                <>
                    <input
                        type="text"
                        value={testName}
                        onChange={(e) => setTestName(e.target.value)}
                        className="input-class"
                        placeholder="Название теста"
                    />
                    <button onClick={handleCreateTest} className="login-button">Создать тест</button>
                    <button onClick={() => navigate('/dashboard')} className="logout-button">Назад</button>
                </>
            ) : (
                <>
                    <h3>Добавьте вопросы для теста: {testName}</h3>
                    {questions.map((q, index) => (
                        <div key={index}>
                            <input
                                type="text"
                                value={q.question}
                                onChange={(e) => {
                                    const newQuestions = [...questions];
                                    newQuestions[index].question = e.target.value;
                                    setQuestions(newQuestions);
                                }}
                                className="input-class"
                                placeholder="Вопрос"
                            />
                            {q.answers.map((answer, ansIndex) => (
                                <input
                                    key={ansIndex}
                                    type="text"
                                    value={answer}
                                    onChange={(e) => {
                                        const newQuestions = [...questions];
                                        newQuestions[index].answers[ansIndex] = e.target.value;
                                        setQuestions(newQuestions);
                                    }}
                                    className="input-class"
                                    placeholder={`Ответ ${ansIndex + 1}`}
                                />
                            ))}
                            <button onClick={() => handleAddAnswer(index)} className="login-button">Добавить ответ</button>
                        </div>
                    ))}
                    <button onClick={handleAddQuestion} className="login-button">Добавить вопрос</button>
                    <button onClick={handleSaveQuestions} className="logout-button">Сохранить вопросы</button>
                    <button onClick={() => navigate('/dashboard')} className="logout-button">Назад</button>
                </>
            )}
        </div>
    );
};

export default CreateTest;